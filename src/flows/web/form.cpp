#include "murk/flows/web.hpp"

#include <boost/beast.hpp>

#include <boost/algorithm/string.hpp>

namespace murk::web {
  std::string multipart_formdata_encode(form_t form);
  form_t multipart_formdata_decode(std::string_view form);

  std::string form_url_encode(form_t form) {
    // BUG: does not escape
    std::string ret;

    for (auto& i : form) {
      ret.append(fmt::format("{}={}&", i.first, i.second));
    }
    ret.pop_back();

    return ret;
  }
  form_t form_url_decode(std::string_view s) {
    // BUG: does not escape
    form_t ret;

    std::vector<std::string> elems;
    boost::split(elems, s, [](char c) { return c == '&'; });

    for (auto& i : elems) {
      auto split_pt = i.find('=');
      if (split_pt == std::string_view::npos)
        throw std::invalid_argument("No value in url encoded form");
      auto iter = i.begin() + split_pt;
      ret.emplace(std::string{i.begin(), iter}, std::string{iter + 1, i.end()});
    }

    return ret;
  }
}
