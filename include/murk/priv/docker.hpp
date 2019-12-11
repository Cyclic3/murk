#pragma once

#include <fmt/format.h>

namespace murk::priv {
  std::string docker(std::string_view prog = "") {
    return fmt::format("docker run --privileged --rm -v /:/pwn -it cyclic3/pwn {}", prog);
  }
}
