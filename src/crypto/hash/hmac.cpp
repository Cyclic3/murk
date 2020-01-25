#pragma once

#include "murk/crypto/hash.hpp"

#include <cppthings/iota_iter.hpp>

#include <atomic>
#include <thread>

namespace murk::crypto {
  murk::data gen_hmac_key_collision(Botan::HashFunction& hf,
                                    flow_t<murk::data_const_ref, bool> is_hash_ok,
                                    murk::data_const_ref range,
                                    murk::data_const_ref prefix,
                                    murk::data_const_ref suffix,
                                    size_t scratch_len) {
    if (!scratch_len)
      scratch_len = hf.output_length() + 4;
    std::vector pad(prefix.begin(), prefix.end());
    // A 1 in 8 billion chance of failure (2^-(8*4)) * 50%
    pad.resize(pad.size() + scratch_len);
    pad.insert(pad.end(), suffix.begin(), suffix.end());

    std::vector<std::thread> threads;

    murk::data ret;
    std::atomic_bool flag = false;

    const auto hw_conc = std::thread::hardware_concurrency();

    for (size_t i = 0; i < hw_conc; ++i)
      threads.emplace_back([&, i, pad]() mutable {
        std::unique_ptr<Botan::HashFunction> hasher{hf.clone()};
        auto scratch_begin = pad.begin() + prefix.size();
        auto scratch_end = scratch_begin + scratch_len;
        murk::data result(hf.output_length());

        while (!flag) {
          hasher->update(pad);
          hasher->final(result.data());

          if (is_hash_ok(result) && !flag.exchange(true))
            ret = std::move(result);

        }
      });

    if (!flag)
      throw std::logic_error("Collision not found");

    return ret;
  }
}
