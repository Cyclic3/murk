#pragma once

#include <murk/span.hpp>

#include <boost/asio/ip/udp.hpp>

#include <boost/signals2.hpp>


namespace murk::xflt {
  class dns {
  public:
    struct request {
      std::string req;
      std::string type;
    };

  private:
    boost::asio::ip::udp::socket sock;
    std::array<uint8_t, 512> buf;
    boost::asio::ip::udp::endpoint remote;

  private:
    void start_receive();
  public:
    boost::signals2::signal<void(nonstd::span<const std::string>)> recv;

  public:
    dns(boost::asio::io_context* io_ctx, uint16_t port = 53);
  };
}
