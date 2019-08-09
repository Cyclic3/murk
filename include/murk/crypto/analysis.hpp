#pragma once

#include "murk/flow.hpp"
#include "murk/data.hpp"

#include <gsl/gsl-lite.hpp>

#include <map>

namespace murk::crypto {
  using token_t = size_t;
  using freq_t = double_t;
  using dist_t = std::map<token_t, freq_t>;

  extern const std::array<char, 26> english_lcase;
  extern const std::array<char, 26> english_ucase;
  extern const std::array<char, 10> arabic_numerals;

  extern const std::map<char, freq_t> english_letter_dist;
  extern const std::map<char, freq_t> twist_char_dist;

  inline dist_t dist_conv(const std::map<char, freq_t>& a) {
    dist_t ret;
    for (auto& i : a)
      ret[i.first] = i.second;
    return ret;
  }
//  inline std::map<token_t, char> dist_to_text(const dist_t& a) {
//    std::map<token_t, char> ret;
//    for (auto& i : a)
//      ret[i.first] = i.second;
//    return ret;
//  }

  inline dist_t normalise_freq(const std::map<token_t, size_t>& m) {
    dist_t ret;
    size_t count = 0;

    for (auto i : m)
      count += i.second;

    for (auto i : m)
      ret[i.first] = static_cast<float>(i.second) / static_cast<float>(count);

    return ret;
  }

  inline std::vector<std::pair<token_t, float>> sort_freq(const dist_t& m) {
    std::vector<std::pair<token_t, float>> ret;
    ret.insert(ret.begin(), m.cbegin(), m.cend());
    std::sort(ret.begin(), ret.end(), [](auto a, auto b) { return a.second > b.second; });
    return ret;
  }

  // The smaller this value, the closer the match
  double score_dist_match(const dist_t& expected, const dist_t& measured);

  // The smaller this value, the more likely the
  // measured distribution is a permutation of the expected one
  double score_dist_compare(const dist_t& expected, const dist_t& measured);

  template<typename T>
  constexpr size_t count_set_bits(T t) {
    using tt = std::numeric_limits<T>;
    static_assert(tt::is_integer, "count_set_bits is ill defined on non-ints");
    size_t bits = 0;
    for (size_t j = 0; j < sizeof(t) * CHAR_BIT; ++j) {
      bits += t & 1;
      t >>= 1;
    }
    return bits;
  }

  size_t hamming_distance(murk::data_const_ref a, murk::data_const_ref b);
}
