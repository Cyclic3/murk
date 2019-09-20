#pragma once

//! Universal handler for the various large files that are used in hacking
//!
//! This implementation currentllllly reads the whole thing to memory and copies it
//!
//! TODO: optimise

#include "murk/flows/web.hpp"

#include <tbb/concurrent_unordered_set.h>

#include <shared_mutex>
#include <filesystem>

namespace murk::ext {
  class manager {
  private:
    std::filesystem::path base_dir;
    // TBB doesn't support concurrent removal =(
    std::shared_mutex dls_mutex;
    // Pointers are ok, as it will be maintained for the lifetime of the creating `obtain` call
    std::set<std::string_view> dls;

  public:
    std::filesystem::path obtain(std::string_view file_name, murk::web::http::address addr);
    inline std::filesystem::path obtain(std::string_view file_name, std::string url) {
      return obtain(std::move(file_name), murk::web::http::navigate({url}));
    }
    std::filesystem::path obtain(murk::web::http::address addr) {
      auto file_name_pre = addr.res.find_last_of('/');
      if (file_name_pre == std::string::npos)
        throw std::invalid_argument("murk::ext::manager cannot create name for root resource");

      auto* res_cstr = addr.res.data();

      return obtain({res_cstr + file_name_pre + 1}, addr);
    }
    inline std::filesystem::path obtain(std::string url) {
      return obtain(murk::web::http::navigate({url}));
    }

  public:
    manager();
  };

  extern manager mgr;
}
