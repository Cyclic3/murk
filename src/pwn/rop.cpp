#include "murk/pwn/rop.hpp"

#include <array>
#include <unordered_set>

#include <cstring>
#include <set>

#include <fmt/format.h>

namespace murk::pwn::rop {
  bool effect_acceptable(::cs_insn i) {
    static std::set<std::string_view> acceptables {
      "mov",
      "add",
      "sub",
      "mul",
      "div",
      "pop",
      "push"
    };

    return acceptables.find(i.mnemonic) != acceptables.end();
  }

  std::vector<instructions> get_all_gadgets(const instructions& ins, uint64_t max_effects) {
    std::unordered_set<instructions> ret;
    fmt::print("{}\n", ins.size());

    for (size_t i = 0; i < ins.size(); ++i) {
      if (is_flow_redirect(ins[i])) {
        ret.insert(ins.sub(i, 1));
        for (size_t j = 1; j < std::min(i, max_effects) + 1; ++j) {
          if (effect_acceptable(ins.at(i - j)))
            ret.insert(ins.sub(i - j, j + 1));
          else
            break;
        }
      }
    }

    return {ret.begin(), ret.end()};
  }

//  z3::sort state_t(z3::context& ctx, ::cs_arch arch, ::cs_mode mode) {
//    switch (arch) {
//      case ::CS_ARCH_X86: {
//        if (mode & CS_MODE_64) {
//          constexpr size_t count = 16;

//          static const std::array<const char*, count> names = {
//            "rax", "rcx", "rdx", "rbx", "rsi", "rdi", "rsp", "rbp",
//            "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
//          };

//          static const std::array<z3::sort, count> sorts = {
//            "rax", "rcx", "rdx", "rbx", "rsi", "rdi", "rsp", "rbp",
//            "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
//          };

//          ctx.tuple_sort("state", count, names.data(), sorts.data())
//        }
//      } break;
//    }
//  }

  z3::model derive_model(nonstd::span<const instructions> gadgets) {
    z3::context ctx;

    auto int_arr = ctx.array_sort(ctx.int_sort(), ctx.int_sort());

    auto regs = ctx.array_sort(ctx.int_sort(), ctx.int_sort());
    auto mem = ctx.array_sort(ctx.int_sort(), ctx.int_sort());
    std::array<z3::sort, 1> sorts = {
      // registers
      ctx.array_sort(ctx.int_sort(), ctx.int_sort())//,
      // memory
//      ctx.array_sort(ctx.int_sort(), ctx.int_sort())
    };

    for (auto& gadget : gadgets) {
    }

    for (auto& i : gadgets) {
      auto f = ctx.function(ctx.int_symbol(i.at(0).address), 1, &int_arr, int_arr);
//      f.
    }
  }
}
