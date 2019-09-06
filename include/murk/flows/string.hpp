#pragma once

#include <fmt/format.h>

#include <boost/algorithm/string/split.hpp>

#include <regex>

namespace murk {
  std::string extract(std::regex regex, std::string s) {
    std::smatch matches;
    if (!std::regex_search(s, matches, regex))
      throw std::runtime_error("Could not match regex");
    if (matches.size() == 1)
      return matches[0];
    else if (matches.size() == 2)
      return matches[1];
    else
      throw std::runtime_error("Invalid extraction regex. Use at most 1 capture group.");
  }

  bool contains(std::regex regex, std::string s) {
    return std::regex_search(s, regex);
  }

  std::string replace(std::regex regex, std::string replacement, std::string s) {
    return std::regex_replace(s, regex, replacement);
  }

  std::vector<std::string> split(std::string toks, std::string str) {
    std::vector<std::string> ret;
    boost::split(ret, str, [=](char c) { return toks.find(c) != std::string::npos; });
    return ret;
  }

  inline std::string escape_c_char(char c) {
    switch (c) {
      case ('\r'):
        return "\\r";
      case ('\n'):
        return "\\n";
      case ('\''):
      case ('\"'):
      case ('\\'):
        return fmt::format("\\{}", c);
      default:
        if (::isprint(c))
          return fmt::format("{}", c);
        else
          return fmt::format("\\x{:x}", static_cast<unsigned char>(c));
    }
  }
}
