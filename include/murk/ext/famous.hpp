#pragma once

#include "murk/ext/manager.hpp"

#include <string>
#include <vector>

namespace murk::ext::famous {
  /// The rockyou password list
  inline std::filesystem::path rockyou() {
    return mgr.obtain("https://github.com/brannondorsey/naive-hashcat/releases/download/data/rockyou.txt", true);
  }
}
