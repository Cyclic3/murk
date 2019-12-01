#pragma once

#include "murk/pwn/disasm.hpp"

#include "z3++.h"

#include <map>
#include <vector>

namespace murk::pwn::rop {
  // TODO: handle conditional jump
  std::vector<instructions> get_all_gadgets(const instructions& ins, uint64_t max_effects = 2);

  z3::model derive_model(nonstd::span<const instructions> gadgets);
}
