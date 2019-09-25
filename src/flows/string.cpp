#include "murk/flows/string.hpp"

#include <boost/algorithm/string/split.hpp>

namespace murk {
  std::vector<std::string> split(std::string toks, std::string str) {
    std::vector<std::string> ret;
    boost::split(ret, str, [=](char c) { return toks.find(c) != std::string::npos; });
    return ret;
  }
}
