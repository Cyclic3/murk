#include "murk/flows/fs.hpp"

#include <fstream>
#include <iostream>

namespace murk::fs {
  std::filesystem::path home() {
    std::string ret;

    if (const char* s = getenv("MURK_HOME"); s != nullptr)
      return {s};

#ifdef WIN32
    if (const char* s = getenv("USERPROFILE"); s != nullptr)
      return {s};
#else
    if (const char* s = getenv("XDG_CONFIG_HOME"); s != nullptr)
      return {s};
    else if (const char* s = getenv("HOME"); s != nullptr)
      return {s};
#endif

    throw std::runtime_error("Could not find a suitable home. Try setting MURK_HOME");
  }

  std::filesystem::path temp() {
    std::string ret;

    if (const char* s = getenv("MURK_TEMP"); s != nullptr)
      return {s};
    else
      return std::filesystem::temp_directory_path();
  }


  std::string read_all_text(std::string path) {
    std::ifstream in{path, std::ios::in | std::ios::binary};
    if (!in)
      throw std::runtime_error("Could not open file");
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }

  data read_all_bytes(std::string path) {
    std::ifstream in{path, std::ios::in | std::ios::binary};
    if (!in)
      throw std::runtime_error("Could not open file");
    data contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char*>(contents.data()), contents.size());
    in.close();
    return contents;
  }

  void write_all_text(std::string path, std::string_view str) {
    std::ofstream out{path, std::ios::out | std::ios::binary};
    if (!out)
      throw std::runtime_error("Could not open file");
    out.write(str.data(), str.size());
    out.flush();
    out.close();
  }

  void write_all_bytes(std::string path, data_const_ref b) {
    write_all_text(std::move(path), std::string_view{reinterpret_cast<const char*>(b.data()), b.size()});
  }

  std::vector<std::string> read_all_lines(std::string path) {
    std::vector<std::string> ret;

    std::ifstream in{path, std::ios::in | std::ios::binary};

    std::string elem;

    while (std::getline(in, elem)) {
      if (elem.size() && elem.back() == '\r')
        elem.pop_back();
      ret.emplace_back(std::move(elem));
    }

    return ret;
  }
}
