#pragma once

#include <string>
#include <optional>
#include <vector>

namespace murk::web {
  // BUG: does not escape urlencoded stuff

  // F*** you, rfc3986
  //
  /// Like all the parsers, this parses weakly,
  /// and renders strong
  struct uri {
    static std::string encode(std::string_view str);
    static std::string decode(std::string_view str);

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

}
