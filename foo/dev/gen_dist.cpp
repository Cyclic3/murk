#include <murk/crypto/analysis.hpp>
#include <murk/flows/fs.hpp>
#include <murk/flows/string.hpp>
#include <murk/flows/seq.hpp>
#include <murk/flows/web.hpp>

#include <filesystem>

using namespace murk::flow_ops;

int main() {
  std::string twist =
      murk::in<std::string>()
   >> murk::web::http::navigate
   >> murk::web::http::get
   << "https://www.gutenberg.org/cache/epub/730/pg730.txt";

  int precision = std::numeric_limits<murk::crypto::freq_t>::digits10;

  {
    auto f =
        murk::in<std::string>()
        >> murk::cast_span<char, size_t>
        >> murk::count<size_t>
        >> murk::crypto::normalise_freq;

    auto m = f(twist);
    // Because these are more pain than they're worth
    m.erase('\r');
    //  m.erase('\n');

    fmt::print("std::map<char, double> twist_char_dist {{\n");
    for (auto& i : m)
      fmt::print("  {{ '{}', {:.{}} }},\n", murk::escape_c_char(i.first), i.second, precision);
    fmt::print("}};\n");
  }

//  auto f =
//      murk::in<std::string>()
//      >> murk::cast_span<char, size_t>
//      >> murk::crypto::calc_pair_dist
//      << twist;


//  fmt::print("std::map<char, double> twist_pair_dist {{\n");
//  for (auto& i : f) {
//    fmt::print("  {{ '{}', {{\n", murk::escape_c_char(i.first));
//    for (auto j : i.second)
//      fmt::print("    {{ '{}', {:.{}} }},\n", murk::escape_c_char(j.first), j.second, precision);
//    fmt::print("  }} }}, \n");
//  }
//  fmt::print("}};\n");
}
