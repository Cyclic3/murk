#include "murk/pwn/disasm.hpp"

#include <gsl/gsl-lite.hpp>

#include <capstone/capstone.h>

namespace murk::pwn {
  disassembler::disassembler(::cs_arch arch, ::cs_mode mode_) : mode{mode_} {
    check_cs_err(::cs_open(arch, mode, &handle));
    check_cs_err(::cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON));
  }

  disassembler::~disassembler() {
    check_cs_err(cs_close(&handle));
  }

  instructions disassembler::disasm(murk::data_const_ref code,
                                    uint64_t addr, size_t max_count) const {
    ::cs_insn *insn;
    size_t count = ::cs_disasm(handle, code.data(), code.size(), addr, max_count, &insn);

    if (count == 0)
      throw std::invalid_argument("Could not disassemble code");

    return {insn, count, mode};
  }
}
