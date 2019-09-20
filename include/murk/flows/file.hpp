#pragma once

#include "murk/data.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace murk::file {
  extern std::filesystem::path home;

  inline std::string read_all_text(std::string path) {
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

  inline void write_all_text(std::string path, std::string_view str) {
    std::ofstream out{path, std::ios::out | std::ios::binary};
    if (!out)
      throw std::runtime_error("Could not open file");
    out.write(str.data(), str.size());
    out.flush();
    out.close();
  }

  inline void write_all_bytes(std::string path, data_const_ref b) {
    write_all_text(std::move(path), std::string_view{reinterpret_cast<const char*>(b.data()), b.size()});
  }

  // Doesn't work on my sys due to some weird parsing bug
//  inline std::string read_all_text(std::filesystem::path path) {
//    std::ifstream in(path, std::ios::in | std::ios::binary);
//    if (!in)
//      throw std::runtime_error("Could not open file");
//    std::string contents;
//    in.seekg(0, std::ios::end);
//    contents.resize(in.tellg());
//    in.seekg(0, std::ios::beg);
//    in.read(&contents[0], contents.size());
//    in.close();
//    return(contents);
//  }
}
