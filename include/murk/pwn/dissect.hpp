#pragma once

#include "murk/pwn/capstone_iface.hpp"
#include "murk/data.hpp"

#include "cppthings/movable_ptr.hpp"

namespace murk::pwn {
  // Describes the effects of an instruction
  class effect {
  public:
    enum type_t {

    };

  public:
    std::vector<uint64_t> stack;
  };
}
