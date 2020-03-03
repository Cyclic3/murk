#pragma once

#include "murk/flows/string.hpp"

namespace murk::filt {
  inline std::string zealous_escape_c_char(char c) {
    return fmt::format("\\x{:02x}", static_cast<unsigned char>(c));
  }

  inline std::string zealous_escape_c(std::string_view s) {
    std::string ret;
    ret.reserve(s.size() * 3);
    for (auto c : s)
      ret.append(zealous_escape_c_char(c));
    return ret;
  }

  inline std::string zealous_urlencode(std::string_view s) {
    std::string ret;
    auto ins = std::back_inserter(ret);
    ret.reserve(s.size() * 3);
    for (auto c : s)
      fmt::format_to(ins, "%{:02x}", c);
    return ret;
  }
}
