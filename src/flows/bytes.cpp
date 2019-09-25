#include "murk/flows/bytes.hpp"

// FIXME: Yes I am a bad person
#include <boost/beast/core/detail/base64.hpp>

namespace murk {
  std::string base64_encode(data_const_ref b) {
    std::string ret;
    ret.resize(boost::beast::detail::base64::encoded_size(b.size()));
    ret.resize(boost::beast::detail::base64::encode(ret.data(), b.data(), b.size()));
    return ret;
  }
  data base64_decode(std::string_view s) {
    if (s.size() == 0)
      return {};

    data ret;
    ret.resize(boost::beast::detail::base64::decoded_size(s.size()));
    size_t n_read = 0;
    size_t n_writ = 0;
    do {
      while(::isspace(s[n_read]) || s[n_read] == '=')
        ++n_read;
      auto r = boost::beast::detail::base64::decode(ret.data() + n_writ,
                                                    s.data() + n_read, s.size() - n_read);
      n_writ += r.first;
      n_read += r.second;
    }
    while (n_read < s.size());
    ret.resize(n_writ);
    return ret;
  }
}
