#pragma once

#include "murk/web/uri.hpp"
#include "murk/web/form.hpp"

#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/message.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <fmt/format.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>

namespace murk::web::http {
  class failure : public std::exception {
  private:
    int _code;
    std::string _code_str;

  public:
    constexpr int code() const noexcept { return _code; }

  public:
    inline const char* what() const noexcept override {
      return _code_str.c_str();
    }

  public:
    inline failure(int code) :
      _code{code}, _code_str{get_string(_code)} {}

  public:
    static std::string get_string(int code);
  };


  struct remote {
    struct eps_t;

    bool https;
    std::shared_ptr<eps_t> eps;
    std::string stub;

    bool operator==(const remote& other) const;

    inline operator std::string() const {
      return fmt::format("{}://{}", https ? "https" : "http", stub);
    }
  };

  struct address {
    remote base;
    std::string res;

    inline address& operator+=(std::string_view other) {
      res.append(other);
      return *this;
    }
    inline address operator+(std::string_view other) const {
      address ret = *this;
      ret += other;
      return ret;
    }
    inline address operator/(std::string_view other) const {
      return *this + other;
    }

    inline static address combine(remote rem, const uri::resource& res) {
      address ret;
      ret.base = rem;
      ret.res = res.render();
      return ret;
    }

    inline operator std::string() const {
      std::string ret = base;
      ret += res;
      return ret;
    }
  };

  using http_req = boost::beast::http::request<boost::beast::http::string_body>;
  using http_res = boost::beast::http::response<boost::beast::http::string_body>;

  remote resolve(const uri&);
//    inline address navigate(remote rem, const uri::resource& res) {
//      address ret;
//      ret.base = rem;
//      ret.res = res.render();
//      return ret;
//    }

  inline address navigate(const uri& u) {
    return address::combine(resolve(u), u.res);
  }

  struct cookie {
    std::string key;
    std::string value;
    std::map<std::string, std::string> attr;
    std::set<std::string> flags;

    static cookie parse(std::string_view str);
    std::string render_client() const;
    std::string render_server() const;
  };

  struct result {
    std::string body;
    address end_pos;
    boost::beast::http::status status;
    std::map<std::string, cookie> cookies;

    inline operator std::string&() { return body; }
    inline operator const std::string&() const { return body; }
    inline operator std::string_view() const { return body; }
  };

  result request(const remote&, http_req);

  inline result get(const address& addr) {
    return request(addr.base, { boost::beast::http::verb::get, addr.res, 11 });
  }

  inline result post(const address& addr, form_t form) {
    boost::beast::http::request<boost::beast::http::string_body> req {
      boost::beast::http::verb::post,
      addr.res, 11, form_url_encode(form)
    };
    req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
    req.set(boost::beast::http::field::content_length, req.body().size());
    return request(addr.base, std::move(req));
  }

  inline result post_multipart(const address& addr, multiform_t form, std::string_view boundary) {
    boost::beast::http::request<boost::beast::http::string_body> req {
      boost::beast::http::verb::post,
      addr.res, 11, multipart_formdata_encode(form, boundary)
    };
    req.set(boost::beast::http::field::content_type,
            fmt::format("multipart/form-data; boundary={}", boundary));
    req.set(boost::beast::http::field::content_length, req.body().size());
    return request(addr.base, std::move(req));
  }

  /// Really not thread safe
  class client {
  private:
    struct http_t;
    struct https_t;

    using value_t = std::variant<std::monostate, std::unique_ptr<http_t>, std::unique_ptr<https_t>>;

  private:
    remote base;
    value_t val;

  public:
    result request(http_req);
    std::map<std::string, cookie> cookies;

  public:
    inline result get(std::string res) {
      return request({ boost::beast::http::verb::get, res, 11 });
    }

    inline result post(std::string res, form_t form) {
      boost::beast::http::request<boost::beast::http::string_body> req {
        boost::beast::http::verb::post,
        res, 11, form_url_encode(form)
      };
      req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
      req.set(boost::beast::http::field::content_length, req.body().size());
      return request(std::move(req));
    }

    inline result post_multipart(std::string res, multiform_t form, std::string_view boundary) {
      boost::beast::http::request<boost::beast::http::string_body> req {
        boost::beast::http::verb::post,
        res, 11, multipart_formdata_encode(form, boundary)
      };
      req.set(boost::beast::http::field::content_type,
              fmt::format("multipart/form-data; boundary={}", boundary));
      req.set(boost::beast::http::field::content_length, req.body().size());
      return request(std::move(req));
    }

  public:
    client();
    ~client();

    client(remote base);
  };

  class pool {
  private:
    size_t max_elems;
    // A stack allows a "warming" effect
    std::vector<std::unique_ptr<client>> p;
    std::mutex p_mutex;
    std::condition_variable p_condvar;
    std::atomic<bool> closing = false;
    remote base;

  public:
    inline result request(http_req req) {
      if (closing)
        throw std::runtime_error("Attempted to request from closing pool");

      std::unique_ptr<client> r;
      {
        std::unique_lock lock{p_mutex};
        p_condvar.wait(lock, [&] { return p.size() != 0; });
        r = std::move(p.back());
        p.pop_back();
      }
      auto res = r->request(req);
      {
        std::unique_lock lock{p_mutex};
        p.push_back(std::move(r));
        p_condvar.notify_one();
      }
      return res;
    }

  public:
    inline result get(std::string res) {
      return request({ boost::beast::http::verb::get, res, 11 });
    }

    inline result post(std::string res, form_t form) {
      boost::beast::http::request<boost::beast::http::string_body> req {
        boost::beast::http::verb::post,
        res, 11, form_url_encode(form)
      };
      req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
      req.set(boost::beast::http::field::content_length, req.body().size());
      return request(std::move(req));
    }

    inline result post_multipart(std::string res, multiform_t form, std::string_view boundary) {
      boost::beast::http::request<boost::beast::http::string_body> req {
        boost::beast::http::verb::post,
        res, 11, multipart_formdata_encode(form, boundary)
      };
      req.set(boost::beast::http::field::content_type,
              fmt::format("multipart/form-data; boundary={}", boundary));
      req.set(boost::beast::http::field::content_length, req.body().size());
      return request(std::move(req));
    }

  public:
    inline pool(remote base_, size_t max_elems_ = std::thread::hardware_concurrency()) :
      max_elems{max_elems_}, base{std::move(base_)} {
      std::vector<std::thread> threads;
      for (size_t i = 0; i < max_elems; ++i) {
        threads.emplace_back([&] {
          auto c = std::make_unique<client>(base);
          std::unique_lock lock{p_mutex};
          p.emplace_back(std::move(c));
        });
      }
      for (auto& i : threads)
        i.join();
    }
    ~pool() {
      closing = true;
      while (p.size() != max_elems)
        std::this_thread::yield();
    }
  };
}

