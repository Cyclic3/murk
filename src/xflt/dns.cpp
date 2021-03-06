#include "murk/xflt/dns.hpp"

#include <boost/algorithm/string/split.hpp>

namespace murk::xflt {
  using udp = boost::asio::ip::udp;

  enum dns_header_flags : uint16_t {
    QR_query          =  0 <<  7,
    QR_reply          =  1 <<  7,
    QR_mask           =  1 <<  7,

    OPCODE_query      =  0 <<  3,
    OPCODE_iquery     =  1 <<  3,
    OPCODE_status     =  2 <<  3,
    OPCODE_mask       =  3 <<  3,

    AA_mask           =  1 <<  2,

    TC_mask           =  1 <<  1,

    RD_mask           =  1 <<  0,

    RA_mask           =  1 <<  15,

    Z_mask            =  7 <<  12,

    RCODE_noerror     =  0 <<  8,
    RCODE_formerr     =  1 <<  8,
    RCODE_servfail    =  2 <<  8,
    RCODE_nxdomain    =  3 <<  8,
    RCODE_notimp      =  4 <<  8,
    RCODE_refused     =  5 <<  8,
    RCODE_mask        = 15 <<  8,

    mask              = 0xffff
  };

  constexpr std::array<uint8_t, 12> canned_response_base = {
    0x00, 0x00, // Space for ID
    QR_reply, RCODE_servfail >> 8,
    0, 0, // qd
    0, 0, // an
    0, 0, // ns
    0, 0, // ar
  };

  void dns::start_receive() {
    sock.async_receive_from(boost::asio::buffer(buf), remote, [&] (const boost::system::error_code& err, auto len) {
      try {
        if (err || len < 12)
          goto next;

        dns_header_flags flags =
            static_cast<dns_header_flags>(static_cast<uint16_t>(buf[2]) |
                                          static_cast<uint16_t>(buf[3]) << 8);


        uint16_t qdcount = static_cast<uint16_t>(buf[ 5]) | static_cast<uint16_t>(buf[ 4]) << 8;
  //      uint16_t ancount = static_cast<uint16_t>(buf[ 7]) | static_cast<uint16_t>(buf[ 6]) << 8;
  //      uint16_t nscount = static_cast<uint16_t>(buf[ 9]) | static_cast<uint16_t>(buf[ 8]) << 8;
  //      uint16_t arcount = static_cast<uint16_t>(buf[11]) | static_cast<uint16_t>(buf[10]) << 8;

        auto iter = buf.begin() + 12;
        auto end = buf.begin() + len;

        if (iter == end)
          goto next;

        for (size_t i = 0; i < qdcount; ++i) {
          std::vector<std::string> name;
          while (true) {
            if (iter == end)
              goto next;
            uint8_t section_len = *iter++;
            if (section_len == 0)
              break;
            else if (end - iter < section_len)
              goto next;

            auto begin = iter;
            iter += section_len;
            name.emplace_back(reinterpret_cast<char*>(&*begin), section_len);
          }

          recv(name);
        }

        auto canned_response = canned_response_base;
        std::copy(buf.begin(), buf.begin() + 2, canned_response.begin());

        sock.async_send_to(boost::asio::buffer(canned_response), remote, [](auto...) {});
      }
      catch(...) { throw;goto next; }

      next:
      start_receive();
    });
  }

  dns::dns(boost::asio::io_context* io_ctx, uint16_t port) :
    //udp::v6()
    sock{*io_ctx, udp::endpoint{boost::asio::ip::udp::v4(), port}} {
    start_receive();
  }
}
