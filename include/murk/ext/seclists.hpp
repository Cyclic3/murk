#pragma once

#include "murk/ext/manager.hpp"

#include <string>
#include <vector>

namespace murk::ext::seclists {
  inline std::filesystem::path usernames() {
    return mgr.obtain("https://raw.githubusercontent.com/danielmiessler/SecLists/master/Usernames/xato-net-10-million-usernames-dup.txt");
  }
  inline std::string passwords() {
    return mgr.obtain("https://raw.githubusercontent.com/danielmiessler/SecLists/master/Passwords/xato-net-10-million-passwords-dup.txt");
  }
}
