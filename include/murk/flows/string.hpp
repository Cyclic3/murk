#pragma once

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
}
