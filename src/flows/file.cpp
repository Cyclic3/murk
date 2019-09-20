#include "murk/flows/file.hpp"

namespace murk::file {
#ifdef WIN32
  std::filesystem::path home = getenv("USERPROFILE");
#else
  std::filesystem::path home = getenv("HOME");
#endif
}
