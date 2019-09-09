#pragma once

#include "murk/pwn/capstone_iface.hpp"
#include "murk/data.hpp"

#include "cppthings/movable_ptr.hpp"

namespace murk::pwn {
  class instructions {
  private:
    cppthings::movable_ptr<::cs_insn> insn;
    // Does not get reset with move, so do not use in destructor
    // unless insn is valid
    size_t count;

  public:
    inline operator gsl::span<::cs_insn>() { return {insn, count};}
    inline operator gsl::span<const ::cs_insn>() const { return {insn, count};}
    gsl::span<::cs_insn> to_span() { return *this; }
    gsl::span<const ::cs_insn> to_span() const { return *this; }

  public:
    inline instructions() : insn{nullptr}, count{0} {}
    inline instructions(::cs_insn* insn_, size_t count_) : insn{insn_}, count{count_} {};
    ~instructions();
  };

  class disassembler {
  private:
    ::csh handle;

  public:
    instructions disasm(murk::data_const_ref code,
                        uint64_t addr = 0,
                        size_t max_count = 0) const;

  public:
    disassembler(::cs_arch arch, ::cs_mode mode);
    ~disassembler();
  };
}
