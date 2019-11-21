#pragma once

#include "murk/xfer.hpp"

#include <memory>

namespace murk::web {
  // Behaviour is well-defined across threads, but is suboptimal
  class tcp_stream : public byte_stream {
  public:
    struct impl_t;

  private:
    std::string host;
    std::string port;
    std::shared_ptr<impl_t> impl;

  public:
    size_t write(data_const_ref) override;
    size_t read(data_ref) override;
    size_t read(data&, size_t max = std::numeric_limits<size_t>::max()) override;
    using byte_stream::read;
    size_t avail() override;

    inline const std::string& get_host() const { return host; }
    inline const std::string& get_port() const { return port; }

  public:
    inline size_t operator()(data_const_ref b) { return write(b); }
    inline data operator()() { return read(); }

  public:
    tcp_stream(std::string host, std::string port);
  };
}
