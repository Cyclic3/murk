#pragma once

#include <fstream>
#include <filesystem>
#include<iostream>

namespace murk::file {
  inline std::string read_all_text(std::string path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in)
      throw std::runtime_error("Could not open file");
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
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
