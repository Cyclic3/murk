#pragma once

#include "murk/subprocess.hpp"

namespace murk::tool {
  template<typename... Args>
  inline data msfvenom_gen_shellcode(Args&&... args) {
    return subprocess::start_and_read_all_stdout_bytes("msfvenom", "--format=raw", std::forward<Args&&>(args)...);
  }
}
