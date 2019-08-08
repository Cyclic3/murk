#include <murk/crypto/analysis.hpp>
#include <murk/flows/file.hpp>
#include <murk/flows/seq.hpp>

#include <filesystem>

using namespace murk::flow_ops;

int main() {
  auto f =
      murk::in<std::string>()
   >> murk::file::read_all_text
   >> murk::cast_span<char, size_t>
   >> murk::count<size_t>
   >> murk::crypto::normalise_freq;

  auto m = f("/tmp/twist.txt");
  // Because these are more pain than they're worth
  m.erase('\r');
  m.erase('\n');

  int precision = std::numeric_limits<murk::crypto::freq_t>::digits10;

  fmt::print("{{\n");
  for (auto& i : m) {
    std::string c;
    switch (i.first) {
      case ('\r'):
      case ('\n'):
        continue;
      case ('\''):
      case ('\\'): {
        fmt::print("  {{ '\\{}', {:.{}} }},\n", static_cast<char>(i.first), i.second, precision);
      } break;
      default:
        fmt::print("  {{ '{}', {:.{}} }},\n", static_cast<char>(i.first), i.second, precision);
    }
  }
  fmt::print("}}\n");
}
