#pragma once

#include "murk/span.hpp"

#include <boost/endian/conversion.hpp>

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
  inline std::array<uint8_t, sizeof(Int)> to_big_endian(Int i) {
    std::array<uint8_t, sizeof(Int)> ret;
    // It ain't proper C++ without `*reinterpret_cast` ;)
    *reinterpret_cast<Int*>(ret.data()) = boost::endian::native_to_big(i);
    return ret;
  }

  template<typename Int, typename Iter>
  void to_big_endian(Int i, Iter iter) {
    Int i_conv = boost::endian::native_to_big(i);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&i_conv);
    std::copy(ptr, ptr + sizeof(Int), std::move(iter));
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  inline Int from_big_endian(nonstd::span<const uint8_t> b) {
    return boost::endian::big_to_native(*reinterpret_cast<const Int*>(b.data()));
  }

  template<typename Int>
  inline std::array<uint8_t, sizeof(Int)> to_little_endian(Int i) {
    std::array<uint8_t, sizeof(Int)> ret;
    // It ain't proper C++ without `*reinterpret_cast` ;)
    *reinterpret_cast<Int*>(ret.data()) = boost::endian::native_to_little(i);
    return ret;
  }

  template<typename Int, typename Iter>
  void to_little_endian(Int i, Iter iter) {
    Int i_conv = boost::endian::native_to_little(i);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&i_conv);
    std::copy(ptr, ptr + sizeof(Int), std::move(iter));
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  inline Int from_little_endian(nonstd::span<const uint8_t> b) {
    return boost::endian::little_to_native(*reinterpret_cast<const Int*>(b.data()));
  }

  namespace byte_ops {
    std::array<uint8_t, 8> operator""_u64_be(unsigned long long i) {
      return to_big_endian<uint64_t>(i);
    }

    std::array<uint8_t, 8> operator""_u64_le(unsigned long long i) {
      return to_little_endian<uint64_t>(i);
    }

    std::array<uint8_t, 4> operator""_u32_be(unsigned long long i) {
      return to_big_endian<uint32_t>(i);
    }

    std::array<uint8_t, 4> operator""_u32_le(unsigned long long i) {
      return to_little_endian<uint32_t>(i);
    }

    std::array<uint8_t, 2> operator""_u16_be(unsigned long long i) {
      return to_big_endian<uint16_t>(i);
    }

    std::array<uint8_t, 2> operator""_u16_le(unsigned long long i) {
      return to_little_endian<uint16_t>(i);
    }

    std::array<uint8_t, 8> operator""_i64_be(unsigned long long i) {
      return to_big_endian<int64_t>(i);
    }

    std::array<uint8_t, 8> operator""_i64_le(unsigned long long i) {
      return to_little_endian<int64_t>(i);
    }

    std::array<uint8_t, 4> operator""_i32_be(unsigned long long i) {
      return to_big_endian<int32_t>(i);
    }

    std::array<uint8_t, 4> operator""_i32_le(unsigned long long i) {
      return to_little_endian<int32_t>(i);
    }

    std::array<uint8_t, 2> operator""_i16_be(unsigned long long i) {
      return to_big_endian<int16_t>(i);
    }

    std::array<uint8_t, 2> operator""_i16_le(unsigned long long i) {
      return to_little_endian<int16_t>(i);
    }
  }
}
