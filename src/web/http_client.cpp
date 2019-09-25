#include "murk/web/http.hpp"
#include "murk/web/http_eps.hpp"

#include <boost/beast.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/algorithm/string/predicate.hpp>

namespace murk::web::http {
  struct client::http_t {
    boost::asio::io_context io_ctx;
    boost::asio::ip::tcp::socket sock{io_ctx};

    void connect(const remote& rem) {
      boost::asio::connect(sock, rem.eps->val.begin(), rem.eps->val.end());
    }

    inline http_t(const remote& rem) {
      connect(rem);
    }
  };

  struct client::https_t {
    boost::asio::io_context io_ctx;
    boost::asio::ssl::context ssl_ctx{boost::asio::ssl::context::method::tlsv12};
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sock;

    void connect(const remote& rem) {
      boost::asio::connect(sock.next_layer(), rem.eps->val.begin(), rem.eps->val.end());
      sock.handshake(boost::asio::ssl::stream_base::client);
    }

    https_t(const remote& rem);
  };

  client::https_t::https_t(const remote& rem) : sock{io_ctx, ssl_ctx} {
    ssl_ctx.set_default_verify_paths();
    connect(rem);
  }

  result client::request(http_req req) {
    http_res res;
    boost::beast::flat_buffer buf;
    if (!cookies.empty()) {
      std::string set_cookie_str;
      for (auto& i : cookies) {
        set_cookie_str += i.second.render_client();
        set_cookie_str += ';';
      }
      set_cookie_str.pop_back();
      req.set(boost::beast::http::field::cookie, set_cookie_str);
    }

    req.set(boost::beast::http::field::host, base.stub);
    req.set(boost::beast::http::field::connection, "keep-alive");

    std::visit([&](auto& s) {
      if constexpr (std::is_same_v<std::decay_t<decltype(s)>, std::monostate>)
        throw std::invalid_argument("Attempting to request from an uninitialised socket");
      else {
        try {
          boost::beast::http::write(s->sock, req);
          boost::beast::http::read(s->sock, buf, res);
        }
        catch(const boost::system::system_error& e) {
          if (e.code() == boost::beast::http::error::end_of_stream) {
            s->connect(base);
            boost::beast::http::write(s->sock, req);
            boost::beast::http::read(s->sock, buf, res);
          }
          else
            throw;
        }
      }
    }, val);

    result ret;
    ret.status = res.result();

    {
      for (auto iter = res.find(boost::beast::http::field::set_cookie);
           iter != res.end(); ++iter) {
        auto v = iter->value();
        auto c = cookie::parse({v.data(), v.size()});
        ret.cookies[c.key] = c;
      }
    }

    /// XXX: FIXME: NOOOOO
    cookies.merge(ret.cookies);

    // TODO: don't download pointless bodies
    // BUG: does not work cross-site
    switch (ret.status) {
      case (boost::beast::http::status::found):
      case (boost::beast::http::status::moved_permanently):
      case (boost::beast::http::status::permanent_redirect):
      case (boost::beast::http::status::temporary_redirect): {
        auto loc_iter = res.find(boost::beast::http::field::location);
        if (loc_iter == res.end())
          throw std::runtime_error("Bad redirect");

        auto loc = loc_iter->value();
        if (loc.size() == 0)
          throw std::runtime_error("Empty redirect");

          // Redirected out of http client site
        if (boost::contains(loc, "://")) {
          auto new_uri = navigate(loc.to_string());
          if (new_uri.base == base)
            return get(new_uri.res);
          else
            return http::get(navigate(loc.to_string()));
        }
        else if (loc.front() == '/')
          return get(loc.to_string());
        else {
          uri::resource base = req.target().to_string();
          if (!base.path.is_dir)
            base.path.components.pop_back();
          uri::resource extra = loc.to_string();
          base.path.components.insert(base.path.components.begin(),
                              extra.path.components.begin(), extra.path.components.end());
          base.query = std::move(extra.query);
          base.fragment = std::move(extra.fragment);
          return get(base.render());
        }
      } break;

      default: {
        ret.body = res.body();
        ret.end_pos.res = req.target().to_string();
        ret.end_pos.base = base;
        return ret;
      } break;
    }
  }

  client::client() = default;

  client::client(remote base_) : base{std::move(base_)} {
    if (base.https)
      val.emplace<std::unique_ptr<https_t>>(std::make_unique<https_t>(base));
    else
      val.emplace<std::unique_ptr<http_t>>(std::make_unique<http_t>(base));
  }

  client::~client() = default;
}
