#include "murk/web/tcp.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

namespace murk::web {
  struct tcp_stream::impl_t {
    boost::asio::io_context io_ctx;
    boost::asio::ip::tcp::socket s{io_ctx};

    impl_t(std::string_view host, std::string_view port)  {
      boost::asio::ip::tcp::resolver r{io_ctx};
      auto res = r.resolve(host, port);
      boost::asio::connect(s, res);
    }
  };

  size_t tcp_stream::avail() {
    return impl->s.available();
  }

  size_t tcp_stream::write(data_const_ref b) {
    return impl->s.send(boost::asio::buffer(b.data(), b.size()));
  }

  size_t tcp_stream::read(data_ref b) {
    return impl->s.receive(boost::asio::buffer(b.data(), b.size()));
  }

  size_t tcp_stream::read(data& b, size_t max) {
    size_t n_to_read = std::min(impl->s.available(), max);
    size_t offset = b.size();

    b.resize(offset + n_to_read);

    size_t n_read = impl->s.receive(boost::asio::buffer(b.data() + offset, n_to_read));
    b.resize(offset + n_read);
    return n_read;
  }

  data tcp_stream::read(size_t max) {
    data ret;
    read(ret, max);
    return ret;
  }

  tcp_stream::tcp_stream(std::string host_, std::string port_) :
    host{host_}, port{port_}, impl{std::make_unique<impl_t>(host, port)} {}
}