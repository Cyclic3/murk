#include "murk/web/form.hpp"

#include "murk/web/uri.hpp"

#include "murk/flows/string.hpp"

#include <boost/beast.hpp>

#include <boost/algorithm/string.hpp>

namespace murk::web {
  std::string multipart_formdata_encode(const multiform_t& form, std::string_view boundary) {
    std::string ret;

    for (auto& i : form.base) {
      ret.append(fmt::format("--{}\r\nContent-Disposition: form-data; name=\"{}\"\r\n\r\n",
                             boundary, escape_double_quotes(i.first)));
      ret.append(i.second);
      ret.append("\r\n");
    }
    for (auto& i : form.files) {
      ret.append(fmt::format("--{}\r\nContent-Disposition: form-data; name=\"{}\"; filename=\"{}\"\r\nContent-Type: {}\r\n\r\n",
                             boundary, escape_double_quotes(i.first), escape_double_quotes(i.second.filename), escape_double_quotes(i.second.mime)));
      ret.append(reinterpret_cast<const char*>(i.second.dat.data()), i.second.dat.size());
      ret.append("\r\n");
    }
//    ret.erase(ret.end() - 2, ret.end());
    ret.append(fmt::format("--{}--\r\n", boundary));
    return ret;
  }
  multiform_t multipart_formdata_decode(std::string_view form, std::string_view boundary);

  std::string form_url_encode_dodgy(const form_t& form) {
    std::string ret;

    for (auto& i : form) {
      ret.append(fmt::format("{}={}&", i.first, i.second));
    }
    ret.pop_back();

    return ret;
  }

  std::string form_url_encode(const form_t& form) {
    std::string ret;

    for (auto& i : form) {
      ret.append(fmt::format("{}={}&", uri::encode(i.first), uri::encode(i.second)));
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
