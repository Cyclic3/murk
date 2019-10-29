#pragma once

#include "murk/ext/manager.hpp"

namespace murk::ext {
  /// The OUI list of MAC address vendors
  inline std::filesystem::path oui() {
    return mgr.obtain("http://standards-oui.ieee.org/oui/oui.txt", true);
  }
}
