#include "murk/crypto/padding.hpp"

#include <cppthings/defer.hpp>

#include <fmt/color.h>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include <mutex>
#include <condition_variable>

namespace murk::crypto {
  /// TODO: Stop unnecessary alloc for plaintext
  template<bool decrypt>
  data pkcs7_padding_oracle_internal(const flow_t<enciphered_msg, bool>& oracle,
                                     enciphered_msg scapegoat,
                                     log_params log) {

    if (scapegoat.iv.size() != scapegoat.ciphertext.size())
      throw std::invalid_argument("pkcs7_padding_oracle_internal: needs 2 blocks");
    size_t block_size = scapegoat.iv.size();

    data ret(block_size);

    if (log) {
      if (log.binary) {
        for (size_t j = 0; j < block_size * 2; ++j)
          fmt::print(fmt::fg(fmt::color::red), "_");
      }
      else {
        for (size_t j = 0; j < block_size; ++j)
          fmt::print(fmt::fg(fmt::color::red), "_");
      }

      fflush(stdout);
    }
    thread_local boost::asio::thread_pool pool;

    for (size_t i = 0; i < block_size; ++i) {
      auto pos = block_size - i - 1;
      auto padding_value = i + 1;
      // Update from the previous attempt
      for (auto j = pos + 1; j < block_size; ++j) {
        scapegoat.iv[j] ^= padding_value - 1;
        scapegoat.iv[j] ^= padding_value;
      }

      size_t init_value = scapegoat.iv[pos];

      std::mutex done_mutex;
      std::condition_variable done_condvar;
      std::atomic<bool> found = false;
      uint_fast16_t done = 0;
      std::atomic<uint8_t> value;
      auto _ = cppthings::defer([&] {
//        pool.join();
        std::unique_lock lock{done_mutex};
        done_condvar.wait(lock, [&] { return done == 256; });
      });

      for (size_t i = 0; i < 256; ++i) {
        if (found)
          break;
        boost::asio::post(pool, [scapegoat, pos, i, padding_value,
                                 &oracle, &value,
                                 &found, &done_mutex, &done_condvar, &done] () mutable {
          auto _ = cppthings::defer([&]() {
            std::unique_lock lock{done_mutex};
            if (++done == 256) {
              done_condvar.notify_all();
            }
          });
          bool expected_if_first = false;
          scapegoat.iv[pos] = i;
          // Stop chance from screwing us
          if (pos != 0)
            scapegoat.iv[pos-1] ^= padding_value;
          if (found ||
              !oracle(scapegoat) ||
              !found.compare_exchange_strong(expected_if_first, true,
                                             std::memory_order_relaxed, std::memory_order_relaxed)) {
            return;
          }
          // F1RST P0ST
//          pool.stop();
          value = i;
          found = true;
          std::unique_lock lock{done_mutex};
          done_condvar.notify_all();
        });
      }

      std::unique_lock lock{done_mutex};
      done_condvar.wait(lock, [&] { return found.load(); });

      if (!found)
        throw std::invalid_argument("Bad oracle");
      scapegoat.iv[pos] = value;

      if constexpr (decrypt)
        ret[pos] = padding_value ^ value ^ init_value;
      else
        ret[pos] = value ^ padding_value;

      if (log) {
        if (log.binary) {
          std::cout << std::string(block_size * 2, '\b');
          for (size_t j = 0; j < block_size; ++j) {
            if (j < pos)
              fmt::print(fmt::fg(fmt::color::red), "__");
            else {
              fmt::print("{:02x}", static_cast<int>(ret[j]));
            }
          }
        }
        else {
          std::cout << std::string(block_size, '\b'); // clear the current block
          for (size_t j = 0; j < block_size; ++j) {
            if (j < pos)
              fmt::print(fmt::fg(fmt::color::red), "_");
            else if (::isprint(ret[j])) {
              fmt::print("{}", static_cast<char>(ret[j]));
            }
            else {
              fmt::print(fmt::fg(fmt::color::aqua), "#");
            }
          }
        }

        fflush(stdout);
      }
    }

    return ret;
  }

  // Thanks https://blog.skullsecurity.org/2013/padding-oracle-attacks-in-depth
  data pkcs7_padding_oracle_decrypt(flow_t<enciphered_msg, bool> oracle,
                                    std::vector<data> blocks,
                                    log_params log) {
    if (blocks.size() < 2)
      throw std::invalid_argument("Empty ciphertext or missing IV");

    data ret;

    for (; blocks.size() > 1; blocks.erase(blocks.begin())) {
      enciphered_msg scapegoat;
      scapegoat.iv = std::move(blocks[0]);
      scapegoat.ciphertext = blocks[1];
      data plaintext = pkcs7_padding_oracle_internal<true>(oracle, std::move(scapegoat), log);
      ret.insert(ret.end(), plaintext.begin(), plaintext.end());
    }

    if (log) {
      fmt::print("\n");
    }

    return ret;
  }

  // Thanks https://blog.skullsecurity.org/2016/going-the-other-way-with-padding-oracles-encrypting-arbitrary-data
  enciphered_msg pkcs7_padding_oracle_encrypt(flow_t<enciphered_msg, bool> oracle, size_t block_size, data msg, log_params log) {
    // special case
    if (msg.size() == 0) {
      enciphered_msg to_send;
      to_send.iv = data(block_size, 0);
      to_send.ciphertext = data(block_size, 0);
      to_send.iv = pkcs7_padding_oracle_internal<false>(oracle, to_send, {.enabled = log.enabled, .binary = true});
      if (log)
        fmt::print("\n");

      return to_send;
    }

    enciphered_msg ret;

    auto n_unpadded = msg.size() / block_size;

    for (size_t i = 0; i < n_unpadded + 1; ++i) {
      enciphered_msg to_send;
      to_send.iv = data(block_size, 0);
      if (i == 0)
        to_send.ciphertext = data(block_size, 0);
      else
        to_send.ciphertext = data(ret.ciphertext.begin(), ret.ciphertext.begin() + block_size);

      data to_append;
      {
        auto begin = msg.begin() + ((n_unpadded - i) * block_size);
        auto end = (i == 0) ? msg.end() : begin + block_size;
        to_append = data(begin, end);
      }

      if (i == 0)
        to_append.resize(block_size, block_size - (msg.size() % block_size));

      to_send.iv = pkcs7_padding_oracle_internal<false>(oracle, to_send, {.enabled = log.enabled, .binary = true});

      // Add our message to the pad
      std::transform(to_append.begin(), to_append.end(),
                     to_send.iv.begin(),
                     to_send.iv.begin(), std::bit_xor<uint8_t>());

      if (i == n_unpadded && i == 0) {
        ret.iv = to_send.iv;
        ret.ciphertext = to_send.ciphertext;
        break;
      }

      // End block needs the cipertext block
      if (i == 0) {
        // Set the vector up as new
        ret.ciphertext.insert(ret.ciphertext.begin(), to_send.iv.begin(), to_send.iv.end());
        ret.ciphertext.insert(ret.ciphertext.end(), to_send.ciphertext.begin(), to_send.ciphertext.end());
      }
      else if (i == n_unpadded)
        ret.iv = to_send.iv;
      else
        ret.ciphertext.insert(ret.ciphertext.begin(), to_send.iv.begin(), to_send.iv.end());
    }
    if (log)
      fmt::print("\n");

    return ret;
  }
}
