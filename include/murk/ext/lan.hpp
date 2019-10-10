#pragma once

#include "murk/ext/manager.hpp"

namespace murk::ext {
  inline std::filesystem::path oui() {
    return mgr.obtain("http://standards-oui.ieee.org/oui/oui.txt", true);
  }
}
