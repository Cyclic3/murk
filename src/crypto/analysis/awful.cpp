#include "murk/crypto/awful.hpp"

using namespace murk::flow_ops;

namespace murk::crypto {
  namespace xor_single {
    uint8_t crack(const dist_t& expected, data_const_ref ctext) {
      auto score_key =
          murk::in<uint8_t>()
       >> (decrypt > ctext)
       >> murk::cast_span<uint8_t, murk::crypto::token_t>
       >> murk::count<murk::crypto::token_t>
       >> murk::crypto::normalise_freq
       >> (murk::crypto::score_dist_match < (murk::crypto::dist_conv << murk::crypto::twist_char_dist));

      uint8_t best_key = 0;
      murk::crypto::freq_t best_score = std::numeric_limits<murk::crypto::freq_t>::infinity();

      for (size_t i = 0; i < 256; ++i) {
        auto score = score_key(i);
        if (score < best_score) {
          best_key = i;
          best_score = score;
        }
      }

      return best_key;
    }
  }

  namespace xor_vigenere {
    size_t calc_key_length(const dist_t& expected, data_const_ref ctext, size_t min, size_t max) {
      min = std::min<size_t>(2, min);
      max = std::min(ctext.size() / 2, max);

      double best_distance = std::numeric_limits<double>::infinity();
      size_t best_len = min;

      for (size_t i = min; i <= max; ++i) {


        size_t n_blocks = ctext.size() / i;
        double our_distance = 0;
        for (size_t j = 0; j < n_blocks - 1; ++j)
          our_distance += static_cast<double>(hamming_distance(ctext.subspan(j * i, i), ctext.subspan((j + 1) * i, i))) / i;
        our_distance /= (n_blocks - 1);
        if (our_distance < best_distance) {
          best_distance = our_distance;
          best_len = i;
        }
      }

      return best_len;
    }

    murk::data crack_with_known_len(const dist_t& expected, data_const_ref ctext, size_t key_len) {
      std::vector<murk::data> b;
      b.resize(key_len);
      for (size_t i = 0; i < ctext.size(); ++i)
        b[i % key_len].push_back(ctext[i]);

      murk::data key(key_len);

      for (size_t key_pos = 0; key_pos < key_len; ++key_pos) {
        auto& block = b[key_pos];
        key[key_pos] = xor_single::crack(expected, block);
      }

      return key;
    }
  }
}
