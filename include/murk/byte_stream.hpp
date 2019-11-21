#pragma once

#include "murk/data.hpp"

namespace murk {
  /// An unbuffered byte stream
  struct byte_stream {
    virtual size_t write(data_const_ref) = 0;
    virtual size_t read(data_ref) = 0;
    virtual size_t read(data&, size_t max = std::numeric_limits<size_t>::max()) = 0;
    virtual data read(size_t max = std::numeric_limits<size_t>::max()) {
      data b;
      read(b, max);
      return b;
    }
    virtual size_t avail() = 0;

    virtual ~byte_stream() = default;
  };
}
