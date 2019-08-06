#include "murk/flows/web.hpp"

#include <boost/beast.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <regex>
#include <variant>

namespace murk::web::http {
  namespace ssl = boost::asio::ssl;
  using tcp = boost::asio::ip::tcp;
  namespace http = boost::beast::http;

  std::string failure::get_string(int code) {
    auto v = http::obsolete_reason(static_cast<http::status>(code));
    return fmt::format("{}: {}", code, v.to_string());
  }

  /*
  class client::impl {
  private:
    using http_t = tcp::socket;
    struct https_t {
      ssl::context ssl_ctx{ssl::context::method::tlsv12};
      ssl::stream<tcp::socket> stream;

      https_t(tcp::socket&& sock) : stream{std::move(sock), ssl_ctx} {
        ssl_ctx.set_default_verify_paths();
        stream.handshake(ssl::stream_base::client);
      }
    };
  private:
    boost::asio::io_context io_ctx;
    tcp::resolver resolver{io_ctx};
    std::variant<std::monostate, http_t, https_t> underlying{};
    tcp::resolver::results_type resolver_results{};
    uri base;

  private:
    // returns true if https, false if http
    bool resolve() {
      if (!base.auth)
        throw std::invalid_argument("web URI's must have an authority");

      // Parts to extract
      bool https;
      std::string port;

      // Well-formed checks
      if (base.scheme == "http") {
        https = false;
        port = base.port().value_or("80");
      }
      else if (base.scheme == "https") {
        https = true;
        port = base.port().value_or("443");
      }
      else
        throw std::invalid_argument("http connect with non-http scheme");

      if (auto i = std::stoi(port); i < 0 || i > 65535)
        throw std::invalid_argument("Port out of range");

      resolver_results = resolver.resolve(base.host(), port);

      return https;
    }

    void connect() {
       return connect(std::holds_alternative<https_t>(underlying));
    }

    /// Leave me to be called when stuff broke
    void connect(bool https) {
      tcp::socket sock{io_ctx};

      boost::asio::connect(sock, resolver_results.begin(), resolver_results.end());

      if (https)
        underlying.emplace<https_t>(std::move(sock));
      else
        underlying.emplace<http_t>(std::move(sock));
    }

  public:
    http_result request(http::request<http::string_body> req) {
      req.content_length(req.body().size());

      std::visit([&](auto& i) {
        using T = std::decay_t<decltype(i)>;
        if constexpr (std::is_same_v<T, std::monostate>)
          throw std::runtime_error("Tried to request through an uninitialised socket");
        else if constexpr (std::is_same_v<T, https_t>) {
          try { http::write(i.stream, req); }
          catch(...) { connect(); http::write(i.stream, req); }
        }
        else {
          try { http::write(i, req); }
          catch(...) { connect(); http::write(i, req); }
        }
      }, underlying);

      http::response<http::string_body> res;
      boost::beast::flat_buffer buf;

      std::visit([&](auto& i) {
        using T = std::decay_t<decltype(i)>;
        if constexpr (std::is_same_v<T, std::monostate>)
          throw std::runtime_error("Tried to request through an uninitialised socket");
        else if constexpr (std::is_same_v<T, https_t>) {
          try { http::read(i.stream, buf, res); }
          catch(...) { connect(); http::read(i.stream, buf, res); }
        }
        else {
          try { http::read(i, buf, res); }
          catch(...) { connect(); http::read(i, buf, res); }
        }
      }, underlying);

      http_result ret;
      ret.status = res.result();

      // TODO: don't download pointless bodies
      // BUG: does not work cross-site
      switch (ret.status) {
        case (http::status::found):
        case (http::status::permanent_redirect):
        case (http::status::temporary_redirect): {
          auto loc_iter = res.find(http::field::location);
          if (loc_iter == res.end())
            throw std::runtime_error("Bad redirect");

          return request(create_req(http::verb::get, uri{loc_iter->value().to_string()}));
        } break;

        default: {
          ret.body = res.body();
          ret.end_pos = uri::parse(req.target().to_string());
          return ret;
        } break;
      }
    }

    http::request<http::string_body> create_req(http::verb verb, uri res,
                                                std::string body = "") {
      http::request<http::string_body> ret {
        verb,
        res.render(),
        11,
        std::move(body)
      };

      ret.set(http::field::connection, "keep-alive");
      ret.set(http::field::host, res.auth->render_stub());

      return ret;
    }

    http::request<http::string_body> create_req(http::verb verb, uri::resource res,
                                                std::string body = "") {
      uri full_uri = base;
      res.path.add_base(base.path());
      full_uri.res = std::move(res);

      return create_req(verb, std::move(full_uri), std::move(body));
    }

  public:
    impl(uri base_) : base{std::move(base_)}  {
      connect(resolve());
    }
    impl(const impl& other) : io_ctx{}, resolver_results{other.resolver_results}, base{other.base}  {
      connect();
    }
  };

  client::~client() = default;
  client::client(uri remote) : _impl{std::make_unique<impl>(std::move(remote))} {}
  client::client(const client& other) : _impl{std::make_unique<impl>(*other._impl)} {}

  http_result client::get(uri::resource u) {
    return _impl->request(_impl->create_req(http::verb::get, std::move(u)));
  }

  http_result client::post_url_form(uri::resource u, form_t body) {
    http::request<http::string_body> req =
      _impl->create_req(http::verb::post, std::move(u), form_url_encode(body));
    req.set(http::field::content_type, "application/x-www-form-urlencoded");
    return _impl->request(std::move(req));
  }

//  http_result client::post(uri::resource u, std::string body) {
//    return _impl->request({http::verb::post, _impl->render_location(std::move(u)), 11, body});
//  }
*/

  thread_local boost::asio::io_context resolver_io_ctx;
  thread_local tcp::resolver resolver{resolver_io_ctx};

  remote resolve(const uri& u) {
    remote ret;

    std::string port;

    // Well-formed checks
    if (!u.auth)
      throw std::invalid_argument("web URI's must have an authority");

    if (u.scheme == "http") {
      ret.https = false;
      port = u.auth->port ? *u.auth->port : "80";
    }
    else if (u.scheme == "https") {
      ret.https = true;
      port = u.auth->port ? *u.auth->port : "443";
    }
    else
      throw std::invalid_argument("http connect with non-http scheme");

    if (auto i = std::stoi(port); i < 0 || i > 65535)
      throw std::invalid_argument("Port out of range");

    ret.eps = resolver.resolve(u.auth->host, port);

    ret.stub = u.auth->render_stub();

    return ret;
  }

  result request(const remote& rem, http_req req) {
    req.set(http::field::host, rem.stub);

    boost::asio::io_context io_ctx;
    tcp::socket sock{io_ctx};
    boost::asio::connect(sock, rem.eps.begin(), rem.eps.end());
    http::response<http::string_body> res;
    boost::beast::flat_buffer buf;

    if (rem.https) {
      ssl::context ssl_ctx{ssl::context::method::tlsv12};
      ssl::stream<tcp::socket> stream{std::move(sock), ssl_ctx};
      http::write(stream, req);
      http::read(stream, buf, res);
    }
    else {
      http::write(sock, req);
      http::read(sock, buf, res);
    }

    result ret;
    ret.status = res.result();

    // TODO: don't download pointless bodies
    // BUG: does not work cross-site
    switch (ret.status) {
      case (http::status::found):
      case (http::status::permanent_redirect):
      case (http::status::temporary_redirect): {
        auto loc_iter = res.find(http::field::location);
        if (loc_iter == res.end())
          throw std::runtime_error("Bad redirect");

        auto loc = loc_iter->value();
        if (loc.size() == 0)
          throw std::runtime_error("Empty redirect");

        if (boost::contains(loc, "://"))
          return get(navigate(loc.to_string()));
        else if (loc.front() == '/')
          return get(navigate(rem, loc.to_string()));
        else {
          uri::resource base = req.target().to_string();
          if (!base.path.is_dir)
            base.path.components.pop_back();
          uri::resource extra = loc.to_string();
          base.path.components.insert(base.path.components.begin(),
                              extra.path.components.begin(), extra.path.components.end());
          base.query = std::move(extra.query);
          base.fragment = std::move(extra.fragment);
          return get(navigate(rem, std::move(base)));
        }
      } break;

      default: {
        ret.body = res.body();
        ret.end_pos = navigate(rem, req.target().to_string());
        return ret;
      } break;
    }
  }
}
