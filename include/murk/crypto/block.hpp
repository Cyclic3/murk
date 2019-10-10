#pragma once

#include "murk/flow.hpp"
#include "murk/data.hpp"
#include "murk/crypto/padding.hpp"
#include "murk/flows/seq.hpp"

#include <botan/block_cipher.h>

#include <set>

namespace murk::crypto {
  // Given any decent block size, there should be no repititions.
  //
  // If there is, we win!
  inline bool ecb_detect(murk::data_const_ref ctext, size_t block_size) {
    using namespace murk::flow_ops;

    if (ctext.size() % block_size)
      return false; // Come on...

    const auto chunks = chunk(ctext, block_size);

    std::set<murk::data> s;
    for (auto i : chunks)
      if (!s.emplace(i.begin(), i.end()).second)
        return true;

    return false;
  }

  inline bool ecb_oracle(murk::flow_t<data_const_ref, data> system, size_t block_size) {
    return ecb_detect(system(data(block_size * 4, 0)), block_size);
  }

  inline data ecb_encrypt(data_const_ref b, const Botan::BlockCipher& bc) {
    murk::data ret(b.size());
    bc.encrypt(b.data(), ret.data());
    return ret;
  }
  inline data ecb_decrypt(data_const_ref b, const Botan::BlockCipher& bc) {
    murk::data ret(b.size());
    bc.decrypt(b.data(), ret.data());
    return ret;
  }

  inline data cbc_encrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc) {
    const auto block_size = bc.block_size();
    const auto n_full_blocks = b.size() / block_size;
    const auto remainder_len = b.size() % block_size;
    const auto final_offset = n_full_blocks * block_size;

    if (iv.size() != block_size)
      throw std::invalid_argument("IV length must be the same as the block size");

    data ret(final_offset + block_size);

    std::copy(iv.begin(), iv.end(), ret.begin());

    for (size_t i = 0; i < final_offset; i += block_size) {
      auto src = b.subspan(i, block_size);
      uint8_t* dest_begin = ret.data() + i;
      xor_bytes_inplace({dest_begin, block_size}, src);
      bc.encrypt(dest_begin);
      auto next = dest_begin + block_size;
      std::copy(dest_begin, next, next);
    }

    // Manual padding to stop copying or mallocs
    xor_bytes_inplace(data_ref{ret}.subspan(final_offset, b.size() % block_size), b.subspan(final_offset));
    auto final_padding = data_ref{ret}.subspan(final_offset + remainder_len);
    auto padding_val = static_cast<uint8_t>(final_padding.size());
    for (auto& i : final_padding)
      i ^= padding_val;
    bc.encrypt(ret.data() + final_offset);

    return ret;
  }

  inline data cbc_decrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc) {
    const auto block_size = bc.block_size();

    if (iv.size() != block_size)
      throw std::invalid_argument("IV length must be the same as the block size");
    if (b.size() % block_size)
      throw std::invalid_argument("Blocks must be a multiple of the block size");

    data ret(b.size());

    for (size_t i = 0; i < b.size(); i += block_size) {
      uint8_t* dest_begin = ret.data() + i;
      bc.decrypt(b.data() + i, dest_begin);

      data_const_ref old_ctext = i ? b.subspan(i - block_size, block_size) : iv;
      xor_bytes_inplace({dest_begin, block_size}, old_ctext);
    }

    pkcs7_remove_inplace(ret);

    return ret;
  }
}
