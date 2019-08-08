#include "murk/flows/web.hpp"

#include <boost/beast.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <regex>
#include <variant>

namespace murk::web::http {
  namespace ssl = boost::asio::ssl;
  using tcp = boost::asio::ip::tcp;
  namespace http = boost::beast::http;

  cookie cookie::parse(std::string_view str) {
    cookie ret;

    std::vector<std::string> toks;
    boost::split(toks, str, boost::is_any_of(",; "));
    if (toks.size() == 0)
      throw std::invalid_argument("Empty cookie");

    {
      auto& i = toks.front();
      auto split_pos = i.find('=');
      if (split_pos == std::string::npos)
        throw std::invalid_argument("No value in cookie");
      ret.key = {i.begin(), i.begin() + split_pos};
      ret.value = {i.begin() + split_pos + 1, i.end()};
    }

    for (auto iter = toks.begin() + 1; iter != toks.end(); ++iter) {
      if (iter->size() == 0)
        continue;
      auto split_pos = iter->find('=');
      if (split_pos == std::string::npos)
        ret.flags.emplace(std::move(*iter));
      else
        ret.attr[{iter->begin(), iter->begin() + split_pos}] =
          {iter->begin() + split_pos + 1, iter->end()};
    }

    return ret;
  }
  std::string cookie::render_client() const {
    return fmt::format("{}={}", key, value);
  }

  std::string cookie::render_server() const {
    std::string ret = render_client();
    if (attr.empty()) {
      return ret;
    }

    ret += ';';

    ret += fmt::format("{}={};", key, value);
    for (auto& i : attr) {
        ret += fmt::format("{}={};", i.first, i.second);
    }
    for (auto& i : flags) {
      ret += i;
      ret += ';';
    }

    ret.pop_back();
    return ret;
  }

  std::string failure::get_string(int code) {
    auto v = http::obsolete_reason(static_cast<http::status>(code));
    return fmt::format("{}: {}", code, v.to_string());
  }

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
      stream.handshake(ssl::stream_base::client);

      http::write(stream, req);
      http::read(stream, buf, res);
    }
    else {
      http::write(sock, req);
      http::read(sock, buf, res);
    }

    result ret;
    ret.status = res.result();

    {
      for (auto iter = res.find(http::field::set_cookie);
           iter != res.end(); ++iter) {
        auto v = iter->value();
        auto c = cookie::parse({v.data(), v.size()});
        ret.cookies[c.key] = c;
      }
    }

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
          return get(address::combine(rem, loc.to_string()));
        else {
          uri::resource base = req.target().to_string();
          if (!base.path.is_dir)
            base.path.components.pop_back();
          uri::resource extra = loc.to_string();
          base.path.components.insert(base.path.components.begin(),
                              extra.path.components.begin(), extra.path.components.end());
          base.query = std::move(extra.query);
          base.fragment = std::move(extra.fragment);
          return get(address::combine(rem, std::move(base)));
        }
      } break;

      default: {
        ret.body = res.body();
        ret.end_pos = address::combine(rem, req.target().to_string());
        return ret;
      } break;
    }
  }
}
