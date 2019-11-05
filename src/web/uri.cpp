#include "murk/web/uri.hpp"

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>

#include <regex>

namespace murk::web {
  std::string uri::authority::render() const {
    std::string ret;

    if (userinfo) {
      ret += *userinfo;
      ret += '@';
    }

    ret += host;

    if (port) {
      ret += ':';
      ret += *port;
    }

    return ret;
  }

  std::string uri::stem::render() const {
    std::string ret;

    for (auto& i : components) {
      ret += '/';
      ret += i;
    }

    if (is_dir)
      ret += '/';

    return ret;
  }

  std::string uri::resource::render() const {
    std::string ret;

    ret += path.render();
    if (query) {
      ret += '?';
      ret += *query;
    }

    if (fragment) {
      ret += '#';
      ret += *fragment;
    }

    return ret;
  }

  std::string uri::render() const {
    std::string ret;

    ret += scheme;
    ret += ":";
    if (auth) {
      ret += "//";
      ret += auth->render();
    }

    ret += res.render();

    return ret;
  }

  uri::authority uri::authority::parse(std::string s) {
    authority ret;

    static std::regex regex {
      "(?:([^@/]+)@)?" // userinfo
      "([^:/]+)"       // host
      "(?::([0-9]+))?" // port
    };

    std::smatch matches;
    if (!std::regex_match(s, matches, regex))
      throw std::invalid_argument("Mangled URI");

    if (matches[1].matched)
      ret.userinfo = matches[1];

    ret.host = matches[2];

    if (matches[3].matched)
      ret.port = matches[3];

    return ret;
  }

  uri::stem uri::stem::parse(std::string s) {
    stem ret;

    if (s.size() == 0) {
      ret.is_dir = true;
      return ret;
    }

    boost::split(ret.components, s, [](char c) { return c == '/'; });

    // Boost adds an extra element for trailing slash
    if (ret.components.back().empty()) {
      ret.components.pop_back();
      ret.is_dir = true;
    }
    else {
      ret.is_dir = false;
    }

    return ret;
  }

  uri::resource uri::resource::parse(std::string s) {
    resource ret;

    static std::regex regex {
      "(?:/([^/][^#?]*))?"    // path
      "(?:\\?([^#]*))?"       // query
      "(?:#(.*))?"            // fragment
    };

    std::smatch matches;
    if (!std::regex_search(s, matches, regex))
      throw std::invalid_argument("Mangled URI");

    ret.path = stem::parse(matches[1]);
    if (matches[2].matched)
      ret.query = matches[2];
    if (matches[3].matched)
      ret.fragment = matches[3];

    return ret;
  }

  uri uri::parse(std::string s) {
    uri ret;

    static std::regex regex {
      "([A-Za-z][A-Za-z0-9]*):" // scheme
      "(?://([^/]+))?"          // authority
      "(.*)"                    // path (allowing empty)
    };

    std::smatch matches;
    if (!std::regex_search(s, matches, regex))
      throw std::invalid_argument("Mangled URI");

    ret.scheme = matches[1];
    if (matches[2].matched)
      ret.auth = authority::parse(matches[2]);
    ret.res = resource::parse(matches[3]);

    return ret;
  }

  void uri::stem::add_base(const stem& base) {
    if (base.components.size() == 0)
      return;

    if (base.is_dir)
      components.insert(components.begin(), base.components.begin(), base.components.end());
    else {
      components.front().insert(0, base.components.back());
      components.insert(components.begin(), base.components.begin(), base.components.end() - 1);
    }
  }

  std::string uri::encode(std::string_view str) {
    std::string ret;
    for (auto i : str) {
      // TODO: Remove unnecessary %-encoding
      if (::isalnum(i))
        ret.push_back(i);
      else
        ret.append(fmt::format("%{:02X}", (unsigned char)i));
    }
    return ret;
  }

  std::string uri::decode(std::string_view str) {
    std::string ret;

    for (auto iter = str.begin(); iter != str.end(); ++iter) {
      if (*iter == '%') {
        std::string s{*++iter, *++iter};
        ret.push_back(static_cast<char>(std::stoi(s, 0, 16)));
      }
      else
        ret.push_back(*iter);
    }
    return ret;
  }
}
