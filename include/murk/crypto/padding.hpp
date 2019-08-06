#pragma once

#include "murk/log.hpp"
#include "murk/flow.hpp"
#include "murk/flows/bytes.hpp"
#include "murk/crypto/params.hpp"

namespace murk::crypto {

  /// @param oracle return true when padding is ok
  data pkcs7_padding_oracle_decrypt(flow_t<enciphered_msg, bool> oracle,
                                    std::vector<data> blocks,
                                    log_params log = {});

  /// @param oracle return true when padding is ok
  inline data pkcs7_padding_oracle_decrypt(flow_t<enciphered_msg, bool> oracle,
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
  inline data pkcs7_padding_oracle_decrypt(flow_t<enciphered_msg, bool> oracle,
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

  inline data pkcs7_add(size_t block_size, data b) {
    size_t padded_size = (b.size() + block_size) / block_size * block_size;

    b.resize(padded_size, block_size - (b.size() % block_size));
    return b;
  }
  inline data pkcs7_remove(data b) {
    b.erase(b.end() - b.back(), b.end());
    return b;
  }

  /// @param oracle return true when padding is ok
  enciphered_msg pkcs7_padding_oracle_encrypt(flow_t<enciphered_msg, bool> oracle,
                                              size_t block_size, data msg,
                                              log_params log = {});
}