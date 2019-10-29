#pragma once

//! Universal handler for the various large files that are used in hacking
//!
//! This implementation currentllllly reads the whole thing to memory and copies it
//!
//! TODO: optimise

#include "murk/web/http.hpp"

#include <tbb/concurrent_unordered_set.h>

#include <shared_mutex>
#include <filesystem>

namespace murk::ext {
  /// A simple system to collect resources, such as cred lists
  class manager {
  private:
    std::filesystem::path base_dir;
    std::filesystem::path tmp_dir;
    // TBB doesn't support concurrent removal =(
    std::shared_mutex dls_mutex;
    // Pointers are ok, as it will be maintained for the lifetime of the creating `obtain` call
    std::set<std::string_view> dls;

  public:
    /// Downloads a file from an http(s) site
    ///
    /// @param file_name A unique name for the resource
    /// @param addr The location of the resource
    /// @param perm Indicates whether the data should be temporarily or permanently stored
    std::filesystem::path obtain(std::string_view file_name, murk::web::http::address addr, bool perm = false);
    /// Downloads a file from an http(s) site
    ///
    /// @param file_name A unique name for the resource
    /// @param url The location of the resource
    /// @param perm Indicates whether the data should be temporarily or permanently stored
    inline std::filesystem::path obtain(std::string_view file_name, std::string url, bool perm = false) {
      return obtain(std::move(file_name), murk::web::http::navigate({url}), perm);
    }
    /// Downloads a file from an http(s) site, using the last part of the address as the name
    ///
    /// @param addr The location of the resource
    /// @param perm Indicates whether the data should be temporarily or permanently stored
    std::filesystem::path obtain(murk::web::http::address addr, bool perm = false) {
      auto u = murk::web::uri::resource{addr.res};
      if (u.path.components.empty())
        throw std::invalid_argument("murk::ext::manager cannot create name for root resources");

      return obtain(u.path.components.back(), addr, perm);
    }
    /// Downloads a file from an http(s) site, using the last part of the address as the name
    ///
    /// @param uri The location of the resource
    /// @param perm Indicates whether the data should be temporarily or permanently stored
    std::filesystem::path obtain(std::string uri, bool perm = false) {
      return obtain(web::http::address{uri}, perm);
    }

  public:
    manager();
  };

  extern manager mgr;
}
