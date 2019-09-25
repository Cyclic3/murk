#pragma once

#include <fmt/format.h>

#include <regex>

namespace murk {
  inline std::string extract(std::regex regex, std::string s) {
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

  inline bool contains(std::regex regex, std::string s) {
    return std::regex_search(s, regex);
  }

  inline std::string replace(std::regex regex, std::string replacement, std::string s) {
    return std::regex_replace(s, regex, replacement);
  }

  inline std::vector<std::string> split(std::string toks, std::string str);

  inline std::string escape_single_quotes(std::string_view s) {
    std::string ret;
    for (auto i : s) {
      if (i == '\'')
        ret.append("'");
      else
        ret.push_back(i);
    }
    return ret;
  }

  inline std::string escape_double_quotes(std::string_view s) {
    std::string ret;
    for (auto i : s) {
      if (i == '"')
        ret.append("\\\"");
      else
        ret.push_back(i);
    }
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

  inline std::string escape_c(std::string_view s) {
    std::string ret;
    for (auto i : s)
      ret.append(escape_c_char(i));
    return ret;
  }
}
