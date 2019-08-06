#pragma once

#include "murk/flow.hpp"

#include <gsl/gsl-lite.hpp>

#include <map>

namespace murk::crypto {
  extern std::array<char, 26> english_lcase;
  extern std::array<char, 26> english_ucase;
  extern std::array<char, 10> arabic_numerals;
  extern std::map<char, float> english_dist;

  template<typename T>
  inline std::map<T, size_t> freq_analysis(gsl::span<const T> s) {
    std::map<T, size_t> ret;
    for (auto i : s)
      ++ret[i];
    return ret;
  }

  template<typename T>
  inline std::map<T, float> normalise_freq(std::map<T, size_t> m) {
    std::map<T, float> ret;
    size_t count = 0;

    for (auto i : m)
      count += i.second;

    for (auto i : m)
      ret[i.first] = static_cast<float>(i.second) / static_cast<float>(count);

    return ret;
  }

//  inline std::vector<std::pair<char, float>> sort_freqs(std::map<char, float> m) {
//    std::vector<std::pair<char, float>> ret;
//    ret.insert(ret.begin(), m.cbegin(), m.cend());
//    std::sort(ret.begin(), ret.end(), [](auto a, auto b) { return a.second > b.second; });
//    return ret;
//  }

  namespace distributions {
    using token_t = size_t;

//    /// @returns A pair of (encoder, decoder)
//    template<typename T>
//    std::pair<std::vector<T>, std::map<T, size_t>> tokenise(gsl::span<const T> s) {
//      std::pair<std::vector<T>, std::map<T, size_t>> ret;

//      for (auto i : s) {
//        if (ret.second.insert(i).second) {
//          ret.first = i;
//        }
//      }
//    }

    inline std::map<token_t, float> calc_elem_distribution(gsl::span<const token_t> i) {
      return normalise_freq(freq_analysis(i));
    }

//    std::vector<std::pair<std::pair<token_t, token_t>, float>> calc_pair_distribution(gsl::span<const token_t> toks) {
//      std::vector<std::pair<std::pair<token_t, token_t>, float>> ret;

//      if (toks.size() == 0)
//        return ret;

//      for (size_t i = 1; i < toks.size())
//    }

    std::map<token_t, std::map<token_t, float>> calc_next_distribution(gsl::span<const size_t> i);

    /// @returns A map of (msg input, dist output)
    std::map<size_t, size_t> match_distribution(std::map<size_t, float> dist, std::map<size_t, float> msg);
  };

}
