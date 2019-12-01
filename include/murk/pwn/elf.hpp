#pragma once

#include "murk/data.hpp"
#include "murk/pwn/disasm.hpp"

#include <libelf.h>
#include <gelf.h>

//#include <boost/iterator/iterator_adaptor.hpp>
//#include <boost/iterator/iterator_categories.hpp>

#include <memory>

namespace murk::pwn {
  class elf {
  private:
    struct ehdr_t {
      GElf_Ehdr ehdr;
    };

    struct phdr_t {
      GElf_Phdr base;
    };

    struct shdr_t {
      GElf_Shdr base;

      template<bool is_const = true>
      inline data_any_ref<is_const> get_ref(data_any_ref<is_const> b) {
        // This does bounds checking
        return b.subspan(base.sh_offset, base.sh_size);
      }
//      data_ref get_ref(data_ref b) {
//        // This does bounds checking
//        return b.subspan(base.sh_offset, base.sh_size);
//      }
    };

//    class phdr_coll_t {
//      friend elf;
//    public:
//      class const_iterator : public boost::iterators::iterator_adaptor<
//          const_iterator,
//          std::vector<GElf_Phdr>::const_iterator,
//          phdr_t,
//          boost::iterators::random_access_traversal_tag
//      > {
//      public:
//        inline const_iterator(std::vector<GElf_Phdr>::const_iterator base) :
//          iterator_adaptor{base} {}
//      };

//    private:
//      std::vector<GElf_Phdr> program_hdrs;

//    public:
//      inline const_iterator cbegin() const { return {program_hdrs.cbegin()}; }
//      inline const_iterator begin() const { return {program_hdrs.cbegin()}; }
//      inline const_iterator cend() const { return {program_hdrs.cend()}; }
//      inline const_iterator end() const { return {program_hdrs.cend()}; }
//    };

  private:
    std::unique_ptr<Elf, std::integral_constant<decltype(&::elf_end), &::elf_end>> base;
  public:
    ehdr_t elf_hdr;
    std::vector<phdr_t> program_hdrs;
    std::vector<shdr_t> section_hdrs;

  public:
    std::pair<::cs_arch, ::cs_mode> get_capstone_machine() const;

  public:
    template<bool is_const = true>
    inline std::vector<data_any_ref<is_const>> get_executable_sections(data_any_ref<is_const> b) {
      std::vector<data_any_ref<is_const>> ret;

      for (auto& i : section_hdrs)
        if (i.base.sh_flags & SHF_EXECINSTR)
          ret.emplace_back(i.get_ref(b));

      return ret;
    }

  public:
    elf(murk::data_const_ref b);
  };
}
