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
    *reinterpret_cast<uint8_t*>(ret.data()) = boost::endian::native_to_big(i);
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
    return boost::endian::big_to_native(*reinterpret_cast<Int*>(b.data()));
  }
}
