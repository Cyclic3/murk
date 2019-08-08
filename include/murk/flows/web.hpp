#pragma once

#include "murk/flow.hpp"

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/utility/string_view.hpp>

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <set>
#include <thread>
#include <variant>
#include <vector>

namespace murk::web {
  // BUG: does not escape urlencoded stuff

  // F*** you, rfc3986
  //
  /// Like all the parsers, this parses weakly,
  /// and renders strong
  struct uri {
    struct authority {
      std::optional<std::string> userinfo;
      std::string host;
      /// Because a port can be of arbritary length. Really.
      std::optional<std::string> port;

      std::string render() const;
      static authority parse(std::string s);

      inline std::string render_stub() const {
        if (port)
          return host + ":" + *port;
        else
          return host;
      }

      inline bool operator==(const authority& other) const {
        return
          userinfo == other.userinfo &&
          host == other.host &&
          port == other.port;
      }
      inline bool operator!=(const authority& other) const {
        return !(*this == other);
      }

      authority() = default;
      authority(std::string s) {
        *this = parse(s);
      }
    };

    struct stem {
      std::vector<std::string> components;
      bool is_dir = true;

      std::string render() const;
      static stem parse(std::string s);

      void add_base(const stem& base);

      inline bool operator==(const stem& other) const {
        return
          is_dir == other.is_dir &&
          components == other.components;
      }
      inline bool operator!=(const stem& other) const {
        return !(*this == other);
      }

      stem() = default;
      stem(std::string s) {
        *this = parse(s);
      }
    };

    struct resource {
      // Can be one of literally 5 different types
      stem path;
      // No specification, yay
      std::optional<std::string> query;
      std::optional<std::string> fragment;

      static resource parse(std::string str);
      std::string render() const;

      inline bool operator==(const resource& other) const {
        return
          path == other.path &&
          query == other.query &&
          fragment == other.fragment;
      }
      inline bool operator!=(const resource& other) const {
        return !(*this == other);
      }

      resource() = default;
      resource(std::string s) {
        *this = parse(s);
      }
    };

    std::string scheme;
    std::optional<authority> auth;
    resource res;

    inline decltype(authority::userinfo)& userinfo() { return auth.value().userinfo; }
    inline const decltype(authority::userinfo)& userinfo() const { return auth.value().userinfo; }
    inline decltype(authority::host)& host() { return auth.value().host; }
    inline const decltype(authority::host)& host() const { return auth.value().host; }
    inline decltype(authority::port)& port() { return auth.value().port; }
    inline const decltype(authority::port)& port() const { return auth.value().port; }
    inline decltype(resource::path)& path() { return res.path; }
    inline const decltype(resource::path)& path() const { return res.path; }
    inline decltype(resource::query)& query() { return res.query; }
    inline const decltype(resource::query)& query() const { return res.query; }
    inline decltype(resource::fragment)& fragment() { return res.fragment; }
    inline const decltype(resource::fragment)& fragment() const { return res.fragment; }

    static uri parse(std::string str);
    std::string render() const;

    inline bool operator==(const uri& other) const {
      return
        scheme == other.scheme &&
        auth == other.auth &&
        res == other.res;
    }
    inline bool operator!=(const uri& other) const {
      return !(*this == other);
    }

    uri() = default;
    uri(std::string s) {
      *this = parse(s);
    }
  };

  using form_t = std::multimap<std::string, std::string>;

  std::string multipart_formdata_encode(form_t form);
  form_t multipart_formdata_decode(std::string_view form);
  std::string form_url_encode(form_t form);
  form_t form_url_decode(std::string_view form);

  namespace http {
    class failure : public std::exception{
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
      bool https;
      boost::asio::ip::tcp::resolver::results_type eps;
      std::string stub;
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

      inline static address combine(remote rem, const uri::resource& res) {
        address ret;
        ret.base = rem;
        ret.res = res.render();
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

    /// Really not thread safe
    class client {
    private:
      struct http_t {
        boost::asio::io_context io_ctx;
        boost::asio::ip::tcp::socket sock{io_ctx};

        inline void connect(const remote& rem) {
          boost::asio::connect(sock, rem.eps.begin(), rem.eps.end());
        }

        inline http_t(const remote& rem) {
          connect(rem);
        }
      };

      struct https_t {
        boost::asio::io_context io_ctx;
        boost::asio::ssl::context ssl_ctx{boost::asio::ssl::context::method::tlsv12};
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sock;

        inline void connect(const remote& rem) {
          boost::asio::connect(sock.next_layer(), rem.eps.begin(), rem.eps.end());
          sock.handshake(boost::asio::ssl::stream_base::client);
        }

        inline https_t(const remote& rem) : sock{io_ctx, ssl_ctx} {
          ssl_ctx.set_default_verify_paths();
          connect(rem);
        }
      };
      using value_t = std::variant<std::monostate, http_t, https_t>;

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

    public:
      client();
      ~client() = default;

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

    public:
      inline pool(remote base_, size_t max_elems_ = std::thread::hardware_concurrency()) :
        max_elems{max_elems_}, base{std::move(base_)} {
        std::vector<std::thread> threads;
        for (auto i = 0; i < max_elems; ++i) {
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
//    /// Really not thread safe
//    class client {
//    private:
//      using http_t = boost::asio::ip::tcp::socket;
//      struct https_t {
//        boost::asio::ssl::context ssl_ctx{boost::asio::ssl::context::method::tlsv12};
//        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream;

//        https_t(boost::asio::ip::tcp::socket&& sock) : stream{std::move(sock), ssl_ctx} {
//          ssl_ctx.set_default_verify_paths();
//          stream.handshake(boost::asio::ssl::stream_base::client);
//        }
//      };

//    private:
//      boost::asio::io_context io_ctx;
//      boost::asio::ip::tcp::resolver resolver{io_ctx};
//      std::variant<std::monostate, http_t, https_t> underlying{};
//      boost::asio::ip::tcp::resolver::results_type resolver_results{};
//      uri base;

//    public:
//      http_result get (uri::resource uri);
//      http_result post_url_form(uri::resource uri, form_t body);

//    public:
//      // Discards query and fragment
//      client(uri remote);
//      ~client();

//      client(const client&);
//      client(client&&) = default;

//      inline client& operator=(const client& c) {
//        return *new(this) client(c);
//      }
//      client& operator=(client&&) = default;
//    };
//  }
}


