#pragma once

#include "murk/data.hpp"

#include <fmt/format.h>

// FIXME: Yes I am a bad person
#include <boost/beast/core/detail/base64.hpp>
#include <boost/archive/basic_archive.hpp>

namespace murk {
  inline std::string base64_encode(data_const_ref b) {
    std::string ret;
    ret.resize(boost::beast::detail::base64::encoded_size(b.size()));
    ret.resize(boost::beast::detail::base64::encode(ret.data(), b.data(), b.size()));
    return ret;
  }
  inline data base64_decode(std::string_view s) {
    data ret;
    ret.resize(boost::beast::detail::base64::decoded_size(s.size()));
    ret.resize(boost::beast::detail::base64::decode(ret.data(), s.data(), s.size()).first);
    return ret;
  }

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

  inline data xor_bytes(data a, data_const_ref b) {
    if (a.size() != b.size())
      throw std::invalid_argument("Cannot XOR differently sized arrays");
    std::transform(a.cbegin(), a.cend(),
                   b.begin(), a.begin(), std::bit_xor<uint8_t>());
    return a;
  }
}
