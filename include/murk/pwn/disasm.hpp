#pragma once

#include "murk/pwn/capstone_iface.hpp"
//#include "murk/pwn/operation.hpp"
#include "murk/data.hpp"

#include <cppthings/movable_ptr.hpp>

#include <cstring>

namespace murk::pwn {
  class instructions {
  private:
    struct data {
      cppthings::movable_ptr<::cs_insn> ptr;
      size_t count;
      ::cs_mode mode;

      inline data(decltype(ptr) ptr_, size_t count_, ::cs_mode mode_) :
        ptr{std::move(ptr_)}, count{count_}, mode{mode_} {}
      inline ~data() {
        if (ptr)
          cs_free(ptr, count);
      }
    };

  private:
    std::shared_ptr<data> dat;
    size_t begin;
    size_t count;

  public:
    constexpr size_t size() const { return count; }
    constexpr size_t offset() const { return begin; }
    constexpr mode_t mode() const { return dat->mode; }

    inline operator gsl::span<::cs_insn>() { return {dat->ptr + begin, size()}; }
    inline operator gsl::span<const ::cs_insn>() const { return {dat->ptr + begin, size()}; }
    inline gsl::span<::cs_insn> to_span() { return *this; }
    inline gsl::span<const ::cs_insn> to_span() const { return *this; }
    inline ::cs_mode get_mode() const { return dat->mode; }
    inline cs_insn& operator[](size_t i) { return *(dat->ptr + begin + i); }
    inline const cs_insn& operator[](size_t i) const { return *(dat->ptr + begin + i); }
    inline cs_insn& at(size_t i) {
      if (i > size())
        throw std::out_of_range{"An instruction outside the buffer was requested"};
      else
        return operator[](i);
    }
    inline const cs_insn& at(size_t i) const {
      if (i > size())
        throw std::out_of_range{"An instruction outside the buffer was requested"};
      else
        return operator[](i);
    }
    /// XXX: not bounds checked
    inline instructions sub(size_t offset) const {
      const auto new_begin = begin + offset;
      return {dat, new_begin, count - offset};
    }
    /// XXX: not bounds checked
    inline instructions sub(size_t offset, size_t count) const {
      const auto new_begin = begin + offset;
      return {dat, new_begin, count};
    }
    inline instructions all() const {
      return {dat};
    }

  public:
    bool operator==(const instructions& other) const {
      if (mode() != other.mode()|| size() != other.size())
        return false;

      for (size_t i = 0; i < size(); ++i) {
        const auto& a = operator[](i);
        const auto& b = other[i];
        if (a.size != b.size || memcmp(a.bytes, b.bytes, a.size))
          return false;
      }

      return true;
    }
    bool operator!=(const instructions& other) const {
      return !(*this == other);
    }

  public:
    instructions() = default;
    inline instructions(decltype(dat) dat_) :
      dat{std::move(dat_)}, begin{0}, count{dat->count} {}
    inline instructions(decltype(dat) dat_, size_t begin_, size_t count_) :
      dat{std::move(dat_)}, begin{begin_}, count{count_} {}
    inline instructions(::cs_insn* insn_, size_t count_, ::cs_mode mode_) :
      instructions{std::make_unique<data>(insn_, count_, mode_), 0, count_} {};
  };

  class disassembler {
  private:
    ::csh handle;
    ::cs_mode mode;

  public:
    instructions disasm(murk::data_const_ref code,
                        uint64_t addr = 0,
                        size_t max_count = 0) const;

  public:
    disassembler(::cs_arch arch, ::cs_mode mode);
    ~disassembler();
  };

  inline bool is_flow_redirect(const::cs_insn& insn) {
    return !::strcmp(insn.mnemonic, "ret") ||
           !::strcmp(insn.mnemonic, "int") ||
           !::strcmp(insn.mnemonic, "syscall");
  }
}

namespace std {
  template<>
  struct hash<::cs_insn> {
    using argument_type = ::cs_insn;
    using result_type = std::size_t;
    result_type operator()(const argument_type& x) const noexcept {
      size_t ret = 0;
      for (size_t i = 0; i < x.size; ++i)
        ret += static_cast<size_t>(x.bytes[i]) << (i % (std::numeric_limits<size_t>::digits / 8));
      return ret;
    }
  };
  template<>
  struct hash<::murk::pwn::instructions> {
    using argument_type = ::murk::pwn::instructions;
    using result_type = std::size_t;
    result_type operator()(const argument_type& x) const noexcept {
      size_t ret = 0;
      for (size_t i = 0; i < x.size(); ++i)
        ret += std::hash<::cs_insn>{}(x[i]);
      return ret;
    }
  };
}
