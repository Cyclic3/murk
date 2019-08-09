#pragma once

#include <gsl/gsl-lite.hpp>

#include <vector>

#include <cstdint>

namespace murk {
  using data = std::vector<uint8_t>;
  using data_ref = gsl::span<uint8_t>;
  using data_const_ref = gsl::span<const uint8_t>;

  inline data serialise(std::string_view s) {
    return {s.begin(), s.end()};
  }

  inline data_const_ref get_data_const_ref(std::string_view s) {
    data_const_ref ret{reinterpret_cast<const uint8_t*>(s.data()), s.size()};
    return ret;
  }

  template<typename T>
  T deserialise(data_const_ref b);

  template<>
  inline std::string deserialise<std::string>(data_const_ref b) {
    return {b.begin(), b.end()};
  }
}
