#pragma once
#include <string>

namespace murk::web {
  std::string jinja2(std::string_view cmd, std::string delim = "{{'\\x3c\\x62\\x72\\x3e'|safe}}MURKed{{'\\x3c\\x62\\x72\\x3e'|safe}}{{'\\x3c\\x62\\x72\\x3e'|safe}}", bool pretty = true);

  std::string flask_get(std::string_view uri_base, std::string_view cmd);
}
