#pragma once

#include "murk/flow.hpp"
#include "murk/data.hpp"

#include <map>

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

namespace murk::crypto {
  using token_t = size_t;
  using freq_t = double_t;
  using dist_t = std::map<token_t, freq_t>;
  using pair_dist_t = std::map<token_t, std::map<token_t, freq_t>>;
  using token_seq_t = std::vector<token_t>;

  inline pair_dist_t calc_pair_dist(nonstd::span<const token_t> in) {
    if (in.size() < 2)
      return {};

    std::map<token_t, std::map<token_t, size_t>> count;
    for (size_t i = 0; i < in.size() - 1; ++i) {
      ++count[in[i]][in[i + 1]];
    }
    pair_dist_t ret;
    for (auto& i : count)
      for (auto& j : i.second)
        ret[i.first][j.first] = static_cast<freq_t>(j.second) / static_cast<freq_t>(in.size() - 1);
    return ret;
  }

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

    for (auto& i : m)
      count += i.second;

    for (auto& i : m)
      ret[i.first] = static_cast<freq_t>(i.second) / static_cast<freq_t>(count);

    return ret;
  }

  inline dist_t renormalise_freq(const dist_t& dist) {
    dist_t ret = dist;
    freq_t count = 0;

    for (auto& i : ret)
      count += i.second;

    for (auto& i : ret)
      i.second *= count;

    return ret;
  }

  inline std::vector<std::pair<token_t, freq_t>> sort_freq(const dist_t& m) {
    std::vector<std::pair<token_t, freq_t>> ret;
    ret.insert(ret.begin(), m.cbegin(), m.cend());
    std::sort(ret.begin(), ret.end(), [](auto a, auto b) { return a.second > b.second; });
    return ret;
  }

  // The smaller this value, the closer the match
  double score_dist_match(const dist_t& expected, const dist_t& measured);

  // The smaller this value, the more likely the
  // measured distribution is a permutation of the expected one
  double score_dist_compare(const dist_t& expected, const dist_t& measured);

  size_t hamming_distance(murk::data_const_ref a, murk::data_const_ref b);

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
}
