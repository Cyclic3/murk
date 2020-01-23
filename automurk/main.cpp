#include <fmt/format.h>

#include <murk/flow.hpp>

#include <murk/mod.hpp>

int main(int argc, char** argv) {
  if (argc > 1) {
    std::string str;
    // TODO: split strings
    for (int i = 1; i < argc; ++i) {
      str += argv[i];
      str.push_back(' ');
    }
    str.pop_back();
    if (auto ret = murk::mod::module::root().act(str))
      fmt::print("{}\n", *ret);
  }
  else {
    fmt::print(
          "*************************\n"
          "*****    AUTOMURK    ****\n"
          "*************************\n"
          "\n"
          "// Sorry for the inconvenience...\n"
          "\n"
          "\n"
          "Type 'help' to list all modules\n"
          "\n");
    murk::mod::module::root().act("");
  }
}
