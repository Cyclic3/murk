#pragma once

#include "murk/flow.hpp"
#include "murk/data.hpp"
#include "murk/crypto/padding.hpp"
#include "murk/flows/seq.hpp"

#include <botan/block_cipher.h>

#include <set>

namespace murk::crypto {
  /// Looks for block repetitions in the ciphertext,
  /// which have a insignificant chance of occuring in any other mode
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

  /// A function that determines whether an oracle uses ECB mode
  inline bool ecb_oracle(murk::flow_t<data_const_ref, data> oracle, size_t block_size) {
    return ecb_detect(oracle(data(block_size * 4, 0)), block_size);
  }
  
  /// TODO: write this!
//  inline bool ecb_get_block_size(murk::flow_t<data_const_ref, data> oracle) {
//    size_t i = 1;
//    data b;
//    while (true) {
//      if (oracle()
//    }
          
//    return i / 2;
//  }

  /// Encrypts data using the ECB mode
  inline data ecb_encrypt(data_const_ref b, const Botan::BlockCipher& bc) {
    murk::data ret(b.size());
    bc.encrypt_n(b.data(), ret.data(), b.size() / bc.block_size());
    return ret;
  }

  /// Decrypts data using the ECB mode
  inline data ecb_decrypt(data_const_ref b, const Botan::BlockCipher& bc) {
    murk::data ret(b.size());
    bc.decrypt_n(b.data(), ret.data(), b.size() / bc.block_size());
    return ret;
  }

  data ecb_crack_prepend_oracle(threaded_flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte = 'C', log_params log = {});

  // It doesn't matter if we run into the real ptext, as it will have no effect on our block
  inline data ecb_guess_spammed_block(flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte = 'A') {
    size_t min_expected = 2;
    // Let's look for a repeated spam block
    murk::data spam(block_size * 4, spam_byte);

    while(true) {
      auto res = oracle(spam);
      std::map<murk::data, size_t> counts;
      for (auto& i : chunk<const uint8_t>(res, block_size))
        ++counts[{i.begin(), i.end()}];
      const data* potential_ret = nullptr;
      for (auto& i : counts) {
        // Greater than to deal with data replication
        if (i.second > min_expected) {
          // Check if someone beat us to it
          if (potential_ret)
            goto skip;
          else
            potential_ret = &i.first;
        }
      }

      // If we get here, we had at most one candidate
      if (potential_ret)
        return *potential_ret;
      else
        throw std::invalid_argument("Managed to discount valid spam block");

      skip:
      ++min_expected;
      spam.insert(spam.end(), block_size, spam_byte);
    }
  }

  // Returns a tuple of the spam needed, and the resultant data offset
  std::pair<data, size_t> ecb_determine_spam(flow_t<data_const_ref, data> insert_oracle, size_t block_size,
                                             uint8_t spam_byte = 'A', size_t repetitions = 2);

  /// Encrypts arbitrary blocks
  flow_t<data_const_ref, data> ecb_make_block_enc_oracle(flow_t<data_const_ref, data> insert_oracle,
                                                         size_t block_size, std::pair<data, size_t> spam_res);

  /// A specialist adaptor similar to ecb_make_block_enc_oracle, but without removing the end
  inline flow_t<data_const_ref, data> bypass_fixed_prefix_len(flow_t<data_const_ref, data> oracle, std::pair<data, size_t> spam_res) {
    return [oracle{std::move(oracle)}, spam_res{std::move(spam_res)}](data_const_ref b) -> data {
      data in = spam_res.first;
      in.insert(in.end(), b.begin(), b.end());
      auto res = oracle(in);
      res.erase(res.begin(), res.begin() + static_cast<ssize_t>(spam_res.second));
      return res;
    };
  }

  /// Basically repeats until we get what we want!
  ///
  /// Assumes that a double spammed block is not in the prefix, which, if it was truly random or independent of the input,
  /// would be a good assumption. The double spammed block must also not be in the passed data. Sorry.
  /// Use a different spam_byte if this is a problem.
  ///
  /// Also be aware that the prefix must have a non-zero probability of being block aligned. Mess with the oracle until this is the case.
  ///
  /// @param final_spam_byte Must be different from spam_byte
  flow_t<data_const_ref, data> ecb_bypass_random_prefix_len(flow_t<data_const_ref, data> oracle, size_t block_size,
                                                            uint8_t spam_byte = 'B', uint8_t final_spam_byte = 'b', size_t repetitions = 1);

  data cbc_encrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc);

  data cbc_decrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc);

  /// Given a fixed prefix oracle, calculates the spam required to make block_aligned data
  std::pair<data, size_t> cbc_determine_spam(flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte = 'Q');
}
