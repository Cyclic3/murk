#include <gtest/gtest.h>

#include "murk/pwn/disasm.hpp"

#include <fmt/format.h>

int main() {
  murk::pwn::disassembler disasm(CS_ARCH_X86, CS_MODE_64);

  murk::data b = {0x55, 0x48, 0x8b, 0x05, 0xb8, 0x13, 0x00, 0x00};

  auto ins = disasm.disasm(b);

  for (auto& i : ins.to_span()) {
    std::cout << i.mnemonic << " " << i.op_str << std::endl;
  }
}
