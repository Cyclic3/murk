#include "murk/flows/web.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>

namespace murk::web::http {
  result client::request(http_req req) {
    http_res res;
    boost::beast::flat_buffer buf;

    req.set(boost::beast::http::field::host, base.stub);
    req.set(boost::beast::http::field::connection, "keep-alive");

    std::visit([&](auto& s) {
      if constexpr (std::is_same_v<std::decay_t<decltype(s)>, std::monostate>)
        throw std::invalid_argument("Attempting to request from an uninitialised socket");
      else {
        try {
          boost::beast::http::write(s.sock, req);
          boost::beast::http::read(s.sock, buf, res);
        }
        catch(const boost::system::system_error& e) {
          if (e.code() == boost::beast::http::error::end_of_stream) {
            s.connect(base);
            boost::beast::http::write(s.sock, req);
            boost::beast::http::read(s.sock, buf, res);
          }
          else
            throw;
        }
      }
    }, val);

    result ret;
    ret.status = res.result();

    // TODO: don't download pointless bodies
    // BUG: does not work cross-site
    switch (ret.status) {
      case (boost::beast::http::status::found):
      case (boost::beast::http::status::permanent_redirect):
      case (boost::beast::http::status::temporary_redirect): {
        auto loc_iter = res.find(boost::beast::http::field::location);
        if (loc_iter == res.end())
          throw std::runtime_error("Bad redirect");

        auto loc = loc_iter->value();
        if (loc.size() == 0)
          throw std::runtime_error("Empty redirect");

        if (boost::contains(loc, "://"))
          throw std::runtime_error("Redirected out of http client site");
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

    return ret;
  }

  client::client(remote base_) : base{std::move(base_)} {
    if (base.https)
      val.emplace<https_t>(base);
    else
      val.emplace<http_t>(base);
  }
}
