#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  auto ctext = "1c0111001f010100061a024b53535009181c"s;
  auto key = "686974207468652062756c6c277320657965"s;

  auto f =
      (murk::xor_bytes < (murk::hex_decode << ctext) < (murk::hex_decode << key))
   >> murk::hex_encode
   >> (murk::expect<std::string> < "746865206b696420646f6e277420706c6179"s);
  f();
}
