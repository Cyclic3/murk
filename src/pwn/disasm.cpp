#include "murk/pwn/disasm.hpp"

#include <gsl/gsl-lite.hpp>

#include <capstone/capstone.h>

namespace murk::pwn {
  disasm::disasm(::cs_arch arch, ::cs_mode mode) {
    check_cs_err(::cs_open(arch, mode, &handle));
  }

  disasm::~disasm() {
    check_cs_err(cs_close(&handle));
  }

  instructions::~instructions() {
    if (insn)
      cs_free(insn, count);
  }

  instructions disasm::disassemble(murk::data_const_ref code, uint64_t addr, size_t max_count) const {
    ::cs_insn *insn;
    size_t count = ::cs_disasm(handle, code.data(), code.size(), addr, max_count, &insn);

    if (count < 0)
      throw std::invalid_argument("Could not disassemble code");

    return {insn, count};
  }
}
