#pragma once

#include "murk/data.hpp"

#include <string>
#include <filesystem>

namespace murk::file {
  std::filesystem::path home();

  std::string read_all_text(std::string path);

  void write_all_text(std::string path, std::string_view str);

  void write_all_bytes(std::string path, data_const_ref b);

  std::vector<std::string> read_all_lines(std::string path);

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
