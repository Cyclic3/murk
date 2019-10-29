#pragma once

#include "murk/web/form.hpp"
#include "murk/flows/string.hpp"
#include "murk/random.hpp"

#include <fmt/format.h>

namespace murk::plod {
  std::string js_xmlhttpreq(std::string_view method, std::string_view url,
                            std::string_view content_type, std::string_view payload) {
    return fmt::format("var r=new XMLHttpRequest();"
                       "r.open('{}','{}');"
                       "r.setRequestHeader('Content-Type','{}');"
                       "r.send('{}')",
                       escape_c(method), escape_c(url), escape_c(content_type), escape_c(payload));
  }
  std::string js_get(std::string_view url) {
    return fmt::format("var r=new XMLHttpRequest();r.open('GET','{}'); r.send()", url);
  }
  std::string js_get(std::string_view url, web::form_t form) {
    return fmt::format("var r=new XMLHttpRequest();r.open('GET','{}?{}'); r.send()",
                       url, web::form_url_encode(form));
  }
  std::string js_post(std::string_view url, web::form_t form) {
    return js_xmlhttpreq("POST", url, "application/x-www-form-urlencoded", web::form_url_encode(form));
  }
  std::string js_post(std::string_view url, web::multiform_t form) {
    return js_xmlhttpreq("POST", url, "multpart/form-data", web::multipart_formdata_encode(form, random::random_alnum(16)));
  }
  constexpr char* js_enum_func = "for(var item in window)if(typeof window[item]=='function')console.log(item.toString());";
  constexpr char* js_dump_func = "for(var item in window){if(typeof window[item]=='function'){var i=item.toString(); console.log(eval(i+'.toString()'))}};";
}
