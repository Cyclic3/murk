#pragma once

#include <array>
#include <algorithm>
#include <random>

namespace murk::random {
  extern thread_local std::mt19937_64 rng;

  extern const std::array<char, 62> alnum;

  template<typename Iter>
  inline Iter rand_from(Iter begin, Iter end) {
    std::uniform_int_distribution<size_t> dist (0, std::distance(begin, end));
    auto n = dist(rng);
    std::advance(begin, n);
    return begin;
  }

  inline std::string random_alnum(size_t count) {
    std::string ret;
    ret.resize(count);
    for (auto& i : ret)
      i = *rand_from(alnum.begin(), alnum.end());
    return ret;
  }
}
