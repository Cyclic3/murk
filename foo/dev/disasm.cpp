#include <gtest/gtest.h>

#include "murk/pwn/elf.hpp"
#include "murk/pwn/rop.hpp"
#include "murk/flows/fs.hpp"

#include <fmt/format.h>

int main() {
  murk::pwn::disassembler disasm(CS_ARCH_X86, CS_MODE_64);

  murk::data b = murk::fs::read_all_bytes("/bin/sh");

  murk::pwn::elf e(b);

  auto a = e.get_executable_sections(b);

//  murk::data b = {0x55, 0x48, 0x8b, 0x05, 0xb8, 0x13, 0x00, 0x00};

  auto ins = disasm.disasm(b);

  auto gadgets = murk::pwn::rop::get_all_gadgets(ins);

  for (auto& g : gadgets) {
    std::cout << "---" << std::endl;
    for (auto& i : g.to_span()) {
      std::cout << i.mnemonic << " " << i.op_str << std::endl;
    }
  }
}
