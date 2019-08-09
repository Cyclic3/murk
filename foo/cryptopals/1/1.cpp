#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  std::string str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
      murk::in<std::string>()
   >> murk::hex_decode
   >> murk::base64_encode
   >> (murk::expect<std::string> < "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t"s)
   << str;
}
