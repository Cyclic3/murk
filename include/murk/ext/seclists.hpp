#pragma once

#include "murk/ext/manager.hpp"

#include <string>
#include <vector>

namespace murk::ext::seclists {
  /// The seclists shortlist of usernames
  inline std::filesystem::path usernames() {
    return mgr.obtain("https://raw.githubusercontent.com/danielmiessler/SecLists/master/Usernames/Names/names.txt", true);
  }
  /// The seclists comprehensive list of usernames
  inline std::filesystem::path usernames_big() {
    return mgr.obtain("https://raw.githubusercontent.com/danielmiessler/SecLists/master/Usernames/xato-net-10-million-usernames-dup.txt", true);
  }
  /// The seclists short list of passwords
  inline std::filesystem::path passwords() {
    return mgr.obtain("https://github.com/danielmiessler/SecLists/raw/master/Passwords/darkweb2017-top10000.txt", true);
  }
  /// The seclists comprehensive list of passwords
  inline std::filesystem::path passwords_big() {
    return mgr.obtain("https://raw.githubusercontent.com/danielmiessler/SecLists/master/Passwords/xato-net-10-million-passwords-dup.txt", true);
  }
}
