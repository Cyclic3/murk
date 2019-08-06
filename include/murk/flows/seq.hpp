#pragma once

#include <gsl/gsl-lite.hpp>

#include <map>

namespace murk {
  template<typename In, typename Out = In>
  inline std::vector<Out> substitute(std::map<In, Out> subs, gsl::span<const In> s) {
    std::vector<Out> ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret),
                   [&](auto i) { return subs[i]; });
    return ret;
  }

  template<typename Elem>
  inline std::vector<Elem> rotate_left(size_t count, gsl::span<const Elem> s) {
    std::vector<Elem> ret;
    count %= s.size();
    std::rotate_copy(s.begin(), s.begin() + count, s.end(), std::back_inserter(ret));
    return ret;
  }

  template<typename Elem>
  inline std::vector<Elem> rotate_right(size_t count, gsl::span<const Elem> s) {
    std::vector<Elem> ret;
    count %= s.size();
    std::rotate_copy(s.begin(), s.end() - count, s.end(), std::back_inserter(ret));
    return ret;
  }

  template<typename Key, typename Value>
  std::map<Key, Value> zip(gsl::span<const Key> keys, gsl::span<const Value> values) {
    if (keys.size() != values.size())
      throw std::invalid_argument("zip: There must be the same number of keys as values");

    std::map<Key, Value> ret;
    for (auto i = 0; i < keys.size(); ++i)
      ret.emplace(keys[i], values[i]);

    return ret;
  }
}
