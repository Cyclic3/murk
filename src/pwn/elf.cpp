#include "murk/pwn/elf.hpp"

#include <fmt/format.h>

namespace murk::pwn {
  static auto check_elf_version() {
    auto ver = ::elf_version(EV_CURRENT);
    if (ver == EV_NONE)
      throw std::logic_error("Incompatible libelf");
    return ver;
  }

  static auto libelf_ver = check_elf_version();


  std::pair<::cs_arch, ::cs_mode> elf::get_capstone_machine() const {
    switch (elf_hdr.ehdr.e_machine) {
      case EM_X86_64: return {::cs_arch::CS_ARCH_X86, ::CS_MODE_64};
      case EM_386: return {::cs_arch::CS_ARCH_X86, ::CS_MODE_32};
      default:
        throw std::invalid_argument("Unknown arch");
    }
  }

  elf::elf(murk::data_const_ref b) :
    base{::elf_memory(const_cast<char*>(reinterpret_cast<const char*>(b.data())), b.size())} {
    if (!base)
      throw std::invalid_argument("Could not load ELF");

    if (::elf_kind(base.get()) != ELF_K_ELF)
      throw std::invalid_argument("Was not passed a ELF file");

    if (!gelf_getehdr(base.get(), &elf_hdr.ehdr))
      throw std::invalid_argument("Could not load ELF header");

    size_t phdr_count;
    if (auto err = elf_getphdrnum(base.get(), &phdr_count))
      throw std::invalid_argument(fmt::format("Could not count ELF program headers: {}", elf_errmsg(err)));

    // unchecked malloc, probably not
//    phdrs.reserve(phdr_count);

    for (size_t i = 0; i < phdr_count; ++i) {
      auto& phdr = program_hdrs.emplace_back();
      if (::gelf_getphdr(base.get(), i, &phdr.base) != &phdr.base)
        throw std::invalid_argument(fmt::format("gelf_getphdr()"));
    }

    size_t shdr_count;
    if (auto err = elf_getshdrnum(base.get(), &shdr_count))
      throw std::invalid_argument(fmt::format("Could not count ELF section headers: {}", elf_errmsg(err)));

    Elf_Scn* scn = NULL;
    while((scn = elf_nextscn(base.get(), scn))) {
      auto& shdr = section_hdrs.emplace_back();
      if ((gelf_getshdr(scn, &shdr.base)) != &shdr.base)
        throw std::invalid_argument(fmt::format("gelf_getshdr()"));
    }
  }
}
