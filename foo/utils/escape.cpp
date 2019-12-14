#pragma once

#include <iostream>
#include <iomanip>

int main() {
  std::string s;
  if (!std::getline(std::cin, s))
    return -1;
  for (auto i : s)
    std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)i;
  std::cout << std::endl;
}
