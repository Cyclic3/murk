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

  /// An unbuffered byte datagram protocol
  struct byte_dgram {
    virtual size_t write(data_const_ref) = 0;
    virtual size_t read(data_ref) = 0;
    virtual data read(size_t max = std::numeric_limits<size_t>::max());
    /// Indicates if there is likely going to be a message soonish
    virtual bool avail() = 0;
    /// Returns 0 if not well defined
    virtual size_t max_buf() { return 0; }

    virtual ~byte_dgram() = default;
  };

  inline byte_stream& operator<<(byte_stream& bs, data_const_ref b) {
    bs.write(b);
    return bs;
  }
}
