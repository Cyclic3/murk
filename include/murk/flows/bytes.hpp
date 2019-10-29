#pragma once

#include "murk/data.hpp"

#include <fmt/format.h>
namespace murk {
  std::string base64_encode(data_const_ref b);
  data base64_decode(std::string_view s);

  inline data hex_decode(std::string_view hex) {
    if (hex.size() % 2 != 0)
      throw std::runtime_error("Hex string of invalid length");
    murk::data ret;
    ret.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i+=2) {
      char tmp_buf[3] = {hex[i], hex[i + 1], '\0'};
      if (auto byte = ::strtoul(tmp_buf, nullptr, 16); byte < 256)
        ret.push_back(byte);
      else
        throw std::invalid_argument("Bad hex string");
    }
    return ret;
  }

  inline std::string hex_encode(data_const_ref b) {
    std::string ret;
    ret.reserve(b.size() * 2);
    for (int i : b) {
      ret.append(fmt::format("{:02x}", i));
    }
    return ret;
  }

  inline data_ref xor_bytes_inplace(data_ref a, data_const_ref b) {
    if (a.size() != b.size())
      throw std::invalid_argument("Cannot XOR differently sized arrays");
    std::transform(a.cbegin(), a.cend(),
                   b.begin(), a.begin(), std::bit_xor<uint8_t>());
    return a;
  }

  inline data xor_bytes(data a, data_const_ref b) {
    if (a.size() != b.size())
      throw std::invalid_argument("Cannot XOR differently sized arrays");
    std::transform(a.cbegin(), a.cend(),
                   b.begin(), a.begin(), std::bit_xor<uint8_t>());
    return a;
  }

  namespace lit_ops {
    inline data operator""_b(const char* cs, size_t len) {
      return serialise(std::string_view{cs, len});
    }
    inline data operator""_b64(const char* cs, size_t len) {
      return base64_decode({cs, len});
    }
    inline data operator""_hex(const char* cs, size_t len) {
      return hex_decode({cs, len});
    }
  }
}
