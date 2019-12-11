#pragma once

#include "murk/span.hpp"

#include <vector>

#include <cstdint>

namespace murk {
  using data = std::vector<uint8_t>;
  using data_ref = nonstd::span<uint8_t>;
  using data_const_ref = nonstd::span<const uint8_t>;

  inline data serialise(std::string_view s) {
    return {s.begin(), s.end()};
  }

  inline data_const_ref get_data_const_ref(std::string_view s) {
    data_const_ref ret{reinterpret_cast<const uint8_t*>(s.data()), static_cast<data_const_ref::index_type>(s.size())};
    return ret;
  }

  template<typename T>
  T deserialise(data_const_ref b);

  template<>
  inline std::string deserialise<std::string>(data_const_ref b) {
    return {b.begin(), b.end()};
  }
  template<>
  inline std::string_view deserialise<std::string_view>(data_const_ref b) {
    return {reinterpret_cast<const char*>(b.data()), static_cast<std::string_view::size_type>(b.size())};
  }

  template<typename Int>
  std::array<uint8_t, sizeof(Int)> to_big_endian(Int i);

  template<typename Int, typename Iter>
  void to_big_endian(Int i, Iter iter) {
    auto buf = to_big_endian<Int>(i);
    std::copy(buf.begin(), buf.end(), std::move(iter));
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  Int from_big_endian(nonstd::span<const uint8_t> b);

  template<typename Int>
  std::array<uint8_t, sizeof(Int)> to_little_endian(Int i);

  template<typename Int, typename Iter>
  void to_little_endian(Int i, Iter iter) {
    auto buf = to_little_endian<Int>(i);
    std::copy(buf.begin(), buf.end(), std::move(iter));
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  Int from_little_endian(nonstd::span<const uint8_t> b);

  namespace byte_ops {
    inline std::array<uint8_t, 8> operator""_u64_be(unsigned long long i) {
      return to_big_endian<uint64_t>(i);
    }

    inline std::array<uint8_t, 8> operator""_u64_le(unsigned long long i) {
      return to_little_endian<uint64_t>(i);
    }

    inline std::array<uint8_t, 4> operator""_u32_be(unsigned long long i) {
      return to_big_endian<uint32_t>(i);
    }

    inline std::array<uint8_t, 4> operator""_u32_le(unsigned long long i) {
      return to_little_endian<uint32_t>(i);
    }

    inline std::array<uint8_t, 2> operator""_u16_be(unsigned long long i) {
      return to_big_endian<uint16_t>(i);
    }

    inline std::array<uint8_t, 2> operator""_u16_le(unsigned long long i) {
      return to_little_endian<uint16_t>(i);
    }

    inline std::array<uint8_t, 8> operator""_i64_be(unsigned long long i) {
      return to_big_endian<int64_t>(i);
    }

    inline std::array<uint8_t, 8> operator""_i64_le(unsigned long long i) {
      return to_little_endian<int64_t>(i);
    }

    inline std::array<uint8_t, 4> operator""_i32_be(unsigned long long i) {
      return to_big_endian<int32_t>(i);
    }

    inline std::array<uint8_t, 4> operator""_i32_le(unsigned long long i) {
      return to_little_endian<int32_t>(i);
    }

    inline std::array<uint8_t, 2> operator""_i16_be(unsigned long long i) {
      return to_big_endian<int16_t>(i);
    }

    inline std::array<uint8_t, 2> operator""_i16_le(unsigned long long i) {
      return to_little_endian<int16_t>(i);
    }
  }
}
