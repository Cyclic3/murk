#pragma once

#include "murk/data.hpp"

#include <map>
#include <string>

namespace murk::web {
  using form_t = std::multimap<std::string, std::string>;
  struct file_form_elem {
    std::string filename;
    std::string mime = "application/octet-stream";
    data dat;
  };

  using file_form_t = std::multimap<std::string, file_form_elem>;

  struct multiform_t {
    form_t base;
    file_form_t files;

    multiform_t() = default;
    inline multiform_t(const form_t& base_) : base{base_} {}
    inline multiform_t(form_t&& base_) : base{std::move(base_)} {}
    inline multiform_t(form_t&& base_, file_form_t&& files_) : base{std::move(base_)}, files{std::move(files_)} {}
  };

  std::string multipart_formdata_encode(const multiform_t& form, std::string_view boundary);
  multiform_t multipart_formdata_decode(std::string_view form, std::string_view boundary);
  std::string form_url_encode(const form_t& form);
  form_t form_url_decode(std::string_view form);
}
