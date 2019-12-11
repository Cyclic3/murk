#include <fmt/format.h>

#include <murk/flow.hpp>

#include <murk/mod.hpp>

int main() {
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
