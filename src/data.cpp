#include "murk/data.hpp"

#include <boost/endian/conversion.hpp>
namespace murk {
  template<typename Int>
  std::array<uint8_t, sizeof(Int)> to_big_endian(Int i) {
    std::array<uint8_t, sizeof(Int)> ret;
    // It ain't proper C++ without `*reinterpret_cast` ;)
    *reinterpret_cast<Int*>(ret.data()) = boost::endian::native_to_big(i);
    return ret;
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  Int from_big_endian(nonstd::span<const uint8_t> b) {
    return boost::endian::big_to_native(*reinterpret_cast<const Int*>(b.data()));
  }

  template<typename Int>
  std::array<uint8_t, sizeof(Int)> to_little_endian(Int i) {
    std::array<uint8_t, sizeof(Int)> ret;
    // It ain't proper C++ without `*reinterpret_cast` ;)
    *reinterpret_cast<Int*>(ret.data()) = boost::endian::native_to_little(i);
    return ret;
  }

  /// XXX: does not check size. Will check when we get std::span
  template<typename Int>
  Int from_little_endian(nonstd::span<const uint8_t> b) {
    return boost::endian::little_to_native(*reinterpret_cast<const Int*>(b.data()));
  }

  template                            uint8_t      from_big_endian<uint8_t>(nonstd::span<const uint8_t> i);
  template                            uint8_t   from_little_endian<uint8_t>(nonstd::span<const uint8_t> i);
  template                             int8_t      from_big_endian< int8_t>(nonstd::span<const uint8_t> i);
  template                             int8_t   from_little_endian< int8_t>(nonstd::span<const uint8_t> i);
  template std::array<uint8_t, sizeof(uint8_t)>      to_big_endian<uint8_t>(                   uint8_t  i);
  template std::array<uint8_t, sizeof(uint8_t)>   to_little_endian<uint8_t>(                   uint8_t  i);
  template std::array<uint8_t, sizeof( int8_t)>      to_big_endian< int8_t>(                    int8_t  i);
  template std::array<uint8_t, sizeof( int8_t)>   to_little_endian< int8_t>(                    int8_t  i);
  template                            uint16_t      from_big_endian<uint16_t>(nonstd::span<const uint8_t> i);
  template                            uint16_t   from_little_endian<uint16_t>(nonstd::span<const uint8_t> i);
  template                             int16_t      from_big_endian< int16_t>(nonstd::span<const uint8_t> i);
  template                             int16_t   from_little_endian< int16_t>(nonstd::span<const uint8_t> i);
  template std::array<uint8_t, sizeof(uint16_t)>      to_big_endian<uint16_t>(                   uint16_t  i);
  template std::array<uint8_t, sizeof(uint16_t)>   to_little_endian<uint16_t>(                   uint16_t  i);
  template std::array<uint8_t, sizeof( int16_t)>      to_big_endian< int16_t>(                    int16_t  i);
  template std::array<uint8_t, sizeof( int16_t)>   to_little_endian< int16_t>(                    int16_t  i);
  template                            uint32_t      from_big_endian<uint32_t>(nonstd::span<const uint8_t> i);
  template                            uint32_t   from_little_endian<uint32_t>(nonstd::span<const uint8_t> i);
  template                             int32_t      from_big_endian< int32_t>(nonstd::span<const uint8_t> i);
  template                             int32_t   from_little_endian< int32_t>(nonstd::span<const uint8_t> i);
  template std::array<uint8_t, sizeof(uint32_t)>      to_big_endian<uint32_t>(                   uint32_t  i);
  template std::array<uint8_t, sizeof(uint32_t)>   to_little_endian<uint32_t>(                   uint32_t  i);
  template std::array<uint8_t, sizeof( int32_t)>      to_big_endian< int32_t>(                    int32_t  i);
  template std::array<uint8_t, sizeof( int32_t)>   to_little_endian< int32_t>(                    int32_t  i);
  template                            uint64_t      from_big_endian<uint64_t>(nonstd::span<const uint8_t> i);
  template                            uint64_t   from_little_endian<uint64_t>(nonstd::span<const uint8_t> i);
  template                             int64_t      from_big_endian< int64_t>(nonstd::span<const uint8_t> i);
  template                             int64_t   from_little_endian< int64_t>(nonstd::span<const uint8_t> i);
  template std::array<uint8_t, sizeof(uint64_t)>      to_big_endian<uint64_t>(                   uint64_t  i);
  template std::array<uint8_t, sizeof(uint64_t)>   to_little_endian<uint64_t>(                   uint64_t  i);
  template std::array<uint8_t, sizeof( int64_t)>      to_big_endian< int64_t>(                    int64_t  i);
  template std::array<uint8_t, sizeof( int64_t)>   to_little_endian< int64_t>(                    int64_t  i);
}
