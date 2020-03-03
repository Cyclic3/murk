#pragma once

//! Simple functions for simple langauges

#include <string>

namespace murk::baby {
  std::string base64_encode(std::string);
  std::string base64_decode(std::string);

  std::string zealous_escape_c(std::string);
  std::string zealous_urlencode(std::string);

  std::string hex_encode(std::string);
  std::string hex_decode(std::string);

  std::string url_encode(std::string);
  std::string url_decode(std::string);

  std::string get_random_bytes(size_t count);

  std::string priv_docker(std::string prog = "");

  std::string png_chunks(std::string plod);

  std::string ssti_jinja2(std::string plod);
}
