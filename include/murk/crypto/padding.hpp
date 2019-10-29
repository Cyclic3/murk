#pragma once

#include "murk/log.hpp"
#include "murk/flow.hpp"
#include "murk/flows/bytes.hpp"
#include "murk/crypto/params.hpp"

namespace murk::crypto {
  /// @param oracle return true when padding is ok
  data pkcs7_padding_oracle_decrypt(threaded_flow_t<enciphered_msg, bool> oracle,
                                    std::vector<data> blocks,
                                    log_params log = {});

  /// @param oracle return true when padding is ok
  inline data pkcs7_padding_oracle_decrypt(threaded_flow_t<enciphered_msg, bool> oracle,
                                           data_const_ref b, size_t block_size,
                                           log_params log = {}) {
    if (b.size() % block_size)
      throw std::invalid_argument("CBC input not divisible by block size");

    std::vector<data> blocks;
    auto iter = b.begin();
    while (iter != b.end()) {
      auto begin = iter;
      iter += block_size;
      blocks.emplace_back(begin, iter);
    }
    return pkcs7_padding_oracle_decrypt(std::move(oracle), std::move(blocks), log);
  }

  /// @param oracle return true when padding is ok
  inline data pkcs7_padding_oracle_decrypt(threaded_flow_t<enciphered_msg, bool> oracle,
                                           data iv, data_const_ref b,
                                           size_t block_size, log_params log = {}) {
    if (b.size() % block_size || iv.size() != block_size)
      throw std::invalid_argument("Invalid CBC input size");

    std::vector<data> blocks;
    blocks.emplace_back(std::move(iv));

    auto iter = b.begin();
    while (iter != b.end()) {
      auto begin = iter;
      iter += block_size;
      blocks.emplace_back(begin, iter);
    }
    return pkcs7_padding_oracle_decrypt(std::move(oracle), std::move(blocks), log);
  }

  inline void pkcs7_add_inplace(data& b, size_t block_size) {
    size_t padded_size = (b.size() + block_size) / block_size * block_size;
    b.resize(padded_size, static_cast<uint8_t>(block_size - (b.size() % block_size)));
  }
  inline data pkcs7_add(data b, size_t block_size) {
    pkcs7_add_inplace(b, block_size);
    return b;
  }
  inline data pkcs7_add(data_const_ref b, size_t block_size) {
    return pkcs7_add(murk::data{b.begin(), b.end()}, block_size);
  }
  inline void pkcs7_add_copy(data_const_ref b, data_ref out) {
    if (b.size() > out.size())
      throw std::invalid_argument("Cannot fit padding");
    auto out_pos = std::copy(b.begin(), b.end(), out.begin());

    std::fill(out_pos, out.end(), out.end() - out_pos);
  }
  inline data_const_ref pkcs7_remove_ref(data_const_ref b) {
    if (b.back() > b.size())
      throw std::invalid_argument("Padding goes off the beginning of the array");
    return { b.begin(), b.size() - b.back() };
  }
  inline void pkcs7_remove_inplace(data& b) {
    if (b.back() > b.size())
      throw std::invalid_argument("Padding goes off the beginning of the array");
    b.erase(b.end() - b.back(), b.end());
  }
  inline data pkcs7_remove(data b) {
    pkcs7_remove_inplace(b);
    return b;
  }
  inline bool pkcs7_validate(data_const_ref b) {
    if (b.empty())
      return false;

    auto n = b.back();
    if (n > b.size())
      return false;

    auto iter = b.rbegin();
    ++iter; // The first byte cannot disprove the validity
    for (size_t i = 1; i < n; ++i, ++iter) {
      if (*iter != n)
        return false;
    }

    return true;
  }

  /// @param oracle return true when padding is ok
  enciphered_msg pkcs7_padding_oracle_encrypt(threaded_flow_t<enciphered_msg, bool> oracle,
                                              size_t block_size, data msg,
                                              log_params log = {});
}
