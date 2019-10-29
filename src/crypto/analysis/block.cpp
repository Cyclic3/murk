#include "murk/crypto/block.hpp"

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

namespace murk::crypto {
  data ecb_crack_prepend_oracle(threaded_flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte, log_params log) {
    // Get the ctext

    boost::asio::thread_pool pool;

    data spam_buf(block_size, spam_byte);

    std::vector<data> offset_ctexts(block_size);

    // Fetch the offset blocks in parallel
    for (size_t byte_no = 0; byte_no < block_size; ++byte_no) {
      boost::asio::post(pool, [&spam_buf, byte_no, block_size, &oracle, &offset_ctexts]() mutable {
        // Adding a block of 0s to the first one makes the calculations a lot cleaner
        offset_ctexts[byte_no] = oracle(data_const_ref{spam_buf}.subspan(0, block_size - byte_no - 1));
      });
    }
    pool.join();

    size_t ctext_size = offset_ctexts.front().size() - block_size;

    data ptext;
    ptext.reserve(ctext_size);

    size_t ptext_size;
    {
      size_t ptext_size_change = 1;
      for (ptext_size_change = 1; ptext_size_change < offset_ctexts.size(); ++ptext_size_change)
        if (offset_ctexts[ptext_size_change].size() != offset_ctexts[ptext_size_change - 1].size())
          break;
      // If we overshot, then we keep the offset (for some reason)
      ssize_t offset = static_cast<ssize_t>(block_size) - static_cast<ssize_t>(ptext_size_change);
      ptext_size = static_cast<size_t>(static_cast<ssize_t>(ctext_size) - offset);
    }

    for (size_t block_begin = 0; block_begin < ctext_size; block_begin += block_size) {
      // work out the subblocks
      for (size_t byte_no = 0; byte_no < block_size; ++byte_no) {
        // Get the offset blocks with the current offset, and seek to where the current block begins
        //
        // We know that everything before this iter is valud
        auto target_iter = offset_ctexts[byte_no].begin() + static_cast<ssize_t>(block_begin);

        // TODO: parallelise the oracle invocation
        // TODO: we can generate a rainbow table for the first byte of the block
        for (size_t guess_byte = 0; guess_byte <= 255; ++guess_byte) {
          // Check if we have hit the end
          if (ptext.size() == ptext_size)
            return ptext;

          // Set up the buffer with the current candidate byte
          spam_buf.back() = static_cast<uint8_t>(guess_byte);
          // See what the actual result is
          auto guess_buf = oracle(spam_buf);
          // spam_buf will contain an encrypted offset block, in which all the values, except for the last are correct
          //
          // Initially this will be spam_bytes, which we used to offset the block initially
          //
          // As we gain more info, this will become the last 15 bytes of real plaintext, followed by a guessed byte
          //
          // We can compare this generated block with the offset one, thanks to ECB block reordering!
          if (std::equal(guess_buf.begin(), guess_buf.begin() + static_cast<ssize_t>(block_size), target_iter)) {
            // We found it
            ptext.push_back(static_cast<uint8_t>(guess_byte));
            // Move the data back a bit
            std::copy(spam_buf.begin() + 1, spam_buf.end(), spam_buf.begin());
            goto next_byte;
          }
        }
        // If we hit here, something has gone wrong
        throw std::invalid_argument("ECB prepend oracle giving inconsistent results");
        next_byte: {}
      }
    }

    // No padding?
    return ptext;
  }

  // Returns a tuple of the spam needed, and the resultant data offset
  std::pair<data, size_t> ecb_determine_spam(flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte, size_t repetitions) {
    std::pair<data, size_t> ret;
    data& spammed_ptext = ret.first = data(block_size * repetitions, spam_byte);
    size_t& data_offset = ret.second = 0;

    auto expected_block = ecb_guess_spammed_block(oracle, block_size, spam_byte);
    const auto spammed_ctext = repeat<uint8_t>(ecb_guess_spammed_block(oracle, block_size, spam_byte), repetitions);

    for(;;spammed_ptext.insert(spammed_ptext.begin(), spam_byte)) {
      auto res = oracle(spammed_ptext);
      auto iter = std::search(res.begin(), res.end(), spammed_ctext.begin(), spammed_ctext.end());
      if (iter == res.end())
        continue;

      // If we get here, then we have the requested state
      spammed_ptext.erase(spammed_ptext.begin(), spammed_ptext.begin() + static_cast<ssize_t>(block_size * 2));
      data_offset = static_cast<size_t>(iter - res.begin());
      break;
    }

    return ret;

//    // Spam bytes until we get a repeated block
//    //
//    // When we get a repeated block, we can subtract 2 block sizes from the spam to get the minimal required
//    //
//    // The offset will be equal to the position of the first repeated block

//    // We know that it is at least 2 block sizes to get a repetition
//    spam.resize(2 * block_size, spam_byte);

//    for(;;spam.emplace_back(spam_byte)) {
//      // Manually check for repetitions so we can get location data
//      auto res = insert_oracle(spam);
//      // This needs to be separate, as the vector would otherwise be deleted
//      auto chunks = chunk<const uint8_t>(res, block_size);

//      // The position of the repetition
//      size_t second_pos = 0;
//      // TODO: optimise
//      std::set<murk::data> s;
//      for (auto& i : chunks) {
//        if (!s.emplace(i.begin(), i.end()).second)
//          goto found;
//        second_pos += block_size;
//      }
//      // If we didn't find a repetition, skip this one
//      continue;
//      found:
//      // The position of the repeatee
//      data_offset = second_pos - block_size;
//      // Remove the fluff
//      spam.resize(spam.size() - (2 * block_size));
//      break;
//    }

//    return ret;
  }

  /// Encrypts arbitrary blocks
  flow_t<data_const_ref, data> ecb_make_block_enc_oracle(flow_t<data_const_ref, data> insert_oracle,
                                                         size_t block_size, std::pair<data, size_t> spam_res) {
    return [insert_oracle{std::move(insert_oracle)}, spam_res{std::move(spam_res)}, block_size](data_const_ref b) -> data {
      if (b.size() % block_size)
        throw std::invalid_argument("The input must be a multiple of block size. Maybe try pading it and adding it to the end?");
      auto buf = spam_res.first;
      buf.insert(buf.end(), b.begin(), b.end());
      auto res = insert_oracle(buf);
      // Less copying if we delete the end first
      res.erase(res.begin() + spam_res.second + b.size(), res.end());
      res.erase(res.begin(), res.begin() + spam_res.second);
      return res;
    };
  }
  data cbc_encrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc) {
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

  data cbc_decrypt(data_const_ref b, data_const_ref iv, const Botan::BlockCipher& bc) {
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

  flow_t<data_const_ref, data> ecb_bypass_random_prefix_len(flow_t<data_const_ref, data> oracle, size_t block_size,
                                                            uint8_t spam_byte, uint8_t final_spam_byte, size_t repetitions) {
    data spammed_ptext(block_size * repetitions, spam_byte);
    auto spammed_ctext = repeat<uint8_t>(ecb_guess_spammed_block(oracle, block_size, spam_byte), repetitions);

    // Add a different final block, so that we can exclude shifts caused by the final byte of the random data
    // being the same as the spammed_ptext
    const auto final_spam = ecb_guess_spammed_block(oracle, block_size, final_spam_byte);
    spammed_ptext.insert(spammed_ptext.end(), block_size, final_spam_byte);
    spammed_ctext.insert(spammed_ctext.end(), final_spam.begin(), final_spam.end());

    return [oracle{std::move(oracle)}, spammed_ptext{std::move(spammed_ptext)}, spammed_ctext{std::move(spammed_ctext)}](data_const_ref b) -> data {
      auto ptext = spammed_ptext;
      ptext.insert(ptext.end(), b.begin(), b.end());

      while (true) {
        auto res = oracle(ptext);
        // If it is aligned, our double spam will come up
        auto iter = std::search(res.begin(), res.end(), spammed_ctext.begin(), spammed_ctext.end());

        if (iter == res.end())
          continue;

        // Now we have got a candidate, delete the prefix and the ctext spam
        res.erase(res.begin(), iter + static_cast<ssize_t>(spammed_ctext.size()));

        return res;
      }
    };
  }

  std::pair<data, size_t> cbc_determine_spam(flow_t<data_const_ref, data> oracle, size_t block_size, uint8_t spam_byte) {
    std::pair<data, size_t> ret;
    auto& buf = ret.first;
    auto& pos = ret.second = std::numeric_limits<size_t>::max();

    data last_res = oracle(buf);
    buf.push_back(spam_byte);
    data res = oracle(buf);
    while(true) {
      size_t this_pos = static_cast<size_t>(std::mismatch(last_res.begin(), last_res.end(), res.begin()).first  - last_res.begin());
      // Block align so that chance bytes aren't a problem
      this_pos = this_pos / block_size * block_size;

      std::swap(this_pos, pos);

      if (pos > this_pos)
        break;

      last_res = std::move(res);
      buf.push_back(spam_byte);
      res = oracle(buf);
    }
    // We went too far =(
    buf.pop_back();
    // We may have overshot by a block, so fix that if that is the case
    if (buf.size() >= block_size) {
      auto needed = buf.size() % block_size;
      auto extra = buf.size() / block_size * block_size;
      buf.resize(needed);
      pos -= extra;
    }

    return ret;
  }
/*

  data ecb_force_insert_oracle(data_const_ref b, flow_t<data_const_ref, data> block_oracle) {
//    if (input.size() % block_size)
//      throw std::invalid_argument("");*/
}
