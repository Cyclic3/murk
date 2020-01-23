#pragma once

#include "murk/flow.hpp"
#include "murk/span.hpp"

#include <map>

namespace murk {
  template<typename In, typename Out = In>
  inline std::vector<Out> substitute(std::map<In, Out> subs, nonstd::span<const In> s) {
    std::vector<Out> ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret),
                   [&](auto i) { return subs[i]; });
    return ret;
  }

  template<typename Elem>
  inline std::vector<Elem> rotate_left(size_t count, nonstd::span<const Elem> s) {
    std::vector<Elem> ret;
    count %= s.size();
    std::rotate_copy(s.begin(), s.begin() + count, s.end(), std::back_inserter(ret));
    return ret;
  }

  template<typename Elem>
  inline std::vector<Elem> rotate_right(size_t count, nonstd::span<const Elem> s) {
    std::vector<Elem> ret;
    count %= s.size();
    std::rotate_copy(s.begin(), s.end() - count, s.end(), std::back_inserter(ret));
    return ret;
  }

  template<typename Key, typename Value>
  inline std::map<Key, Value> zip(nonstd::span<const Key> keys, nonstd::span<const Value> values) {
    if (keys.size() != values.size())
      throw std::invalid_argument("zip: There must be the same number of keys as values");

    std::map<Key, Value> ret;
    for (auto i = 0; i < keys.size(); ++i)
      ret.emplace(keys[i], values[i]);

    return ret;
  }

  template<typename From, typename To>
  inline std::vector<To> cast_span(nonstd::span<const From> in) {
    std::vector<To> ret;
    std::transform(in.begin(), in.end(), std::back_inserter(ret),
                   [](From i) { return static_cast<To>(i); });
    return ret;
  }

  template<typename From, typename To = From>
  inline std::vector<To> translate_span(flow_t<From, To> mapper, nonstd::span<const From> in) {
    std::vector<To> ret;
    std::transform(std::begin(in), std::end(in), std::back_inserter(ret), mapper);
    return ret;
  }

  template<typename A, typename B, typename Iter>
  inline std::map<A, B> tabulate(Iter begin, Iter end, flow_t<A, B> f) {
    std::map<A, B> ret;
    for (; begin != end; ++begin)
      ret.emplace(*begin, f(*begin));
    return ret;
  }

  template<typename A, typename B, typename Range>
  inline std::map<A, B> tabulate(Range range, flow_t<A, B> f) {
    std::map<A, B> ret;
    for (const A& i : range)
      ret.emplace(i, f(i));
    return ret;
  }

  template<typename Iter, typename Comparable, typename T>
  std::pair<T, Comparable> maximum(Iter begin, Iter end, flow_t<T, Comparable> f) {
    std::map<T, Comparable> tbl = tabulate(begin, end, f);
    if (tbl.empty())
      throw std::invalid_argument("Cannot maximise empty set");
    auto iter = tbl.begin();
    auto max_iter = iter;
    for (; iter != tbl.end(); ++iter)
      if (iter->second > max_iter->second)
        max_iter = iter;
    return *max_iter;
  }

  template<typename Range, typename Comparable, typename T>
  std::pair<T, Comparable> maximum(const Range& r, flow_t<T, Comparable> f) {
    const std::map<T, Comparable> tbl = tabulate(r, f);
    if (tbl.empty())
      throw std::invalid_argument("Cannot maximise empty set");
    auto iter = tbl.begin();
    auto max_iter = iter;
    for (; iter != tbl.end(); ++iter)
      if (iter->second > max_iter->second)
        max_iter = iter;
    return *max_iter;
  }

  template<typename From, typename To, typename Iter>
  std::vector<To> map(Iter begin, Iter end, flow_t<From, To> f) {
    std::vector<To> ret;
    for (; begin != end; ++begin)
      ret.push_back(f(*begin));
    return ret;
  }

  template<typename From, typename To, typename Range>
  std::vector<To> map(Range range, flow_t<From, To> f) {
    std::vector<To> ret;
    for (const From& i : range)
      ret.push_back(f(i));
    return ret;
  }

  template<typename T>
  std::vector<nonstd::span<T>> chunk(nonstd::span<T> s, size_t chunk_size) {
    size_t n_full_chunks = s.size() / chunk_size;
    size_t final_offset = n_full_chunks * chunk_size;
    std::vector<nonstd::span<T>> ret;
    ret.reserve(final_offset == s.size() ? n_full_chunks : n_full_chunks + 1);
    for (size_t i = 0; i < final_offset; i += chunk_size)
      ret.emplace_back(s.data() + i, chunk_size);
    ret.emplace_back(s.data() + final_offset, s.end());
    return ret;
  }

//  template<typename From, typename To>
//  std::vector<To> cast_seq(From in) {
//    std::vector<To> ret;
//    std::transform(std::begin(in), std::end(in), std::back_inserter(ret),
//                   [](auto i) { return static_cast<To>(i); });
//    return ret;
//  }

//  template<typename From, typename To = From>
//  std::vector<To> translate_seq(flow_t<typename std::iterator_traits<decltype(std::begin(in))>::value_type, To> mapper, From in) {
//    std::vector<To> ret;
//    std::transform(std::begin(in), std::end(in), std::back_inserter(ret), mapper);
//    return ret;
//  }

  template<typename T>
  inline std::map<T, size_t> count(nonstd::span<const T> s) {
    std::map<T, size_t> ret;
    for (auto i : s)
      ++ret[i];
    return ret;
  }

  template<typename T, typename Iter>
  inline std::vector<T> repeat(Iter begin, Iter end, size_t count) {
    std::vector<T> ret;
    ret.reserve(end - begin);
    for(size_t i = 0; i < count; ++i)
      ret.insert(ret.end(), begin, end);
    return ret;
  }

  template<typename T, typename Range>
  inline std::vector<T> repeat(Range r, size_t count) {
    return repeat<T>(std::begin(r), std::end(r), count);
  }

  template<typename T, typename Range>
  inline std::vector<T> filter(Range r, flow_t<const T&, bool> filt) {
    std::vector<T> ret;
    for (auto& i : r)
      if (filt(i))
        ret.emplace_back(i);
    return ret;
  }

  namespace seq_ops {
    template<typename T, typename A>
    inline std::vector<T>& operator+=(std::vector<T>& a, A&& b) {
      a.insert(a.end(), std::begin(b), std::end(b));
      return a;
    }

    template<typename T, typename A>
    inline std::vector<T> operator+(std::vector<T> a, A&& b) {
      a += b;
      return a;
    }
  }
}
