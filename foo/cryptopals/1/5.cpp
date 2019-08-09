#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  std::array<uint8_t, 3> key = {'I', 'C', 'E'};
  std::string ptext = {
    "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"s
  };
  murk::data ctext;

  for (size_t j = 0; j < ptext.size(); ++j) {
    ctext.push_back(ptext[j] ^ key[j % key.size()]);
  }

  std::cout << murk::hex_encode(ctext) << std::endl;
}
