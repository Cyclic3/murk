#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>
#include <murk/flows/string.hpp>
#include <murk/flows/web.hpp>
#include <murk/crypto/analysis.hpp>
#include <murk/crypto/awful.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  {
    std::string a = "this is a test";
    std::string b = "wokka wokka!!!";

    size_t d = murk::crypto::hamming_distance(murk::get_data_const_ref(a), murk::get_data_const_ref(b));

    if (d != 37)
      throw std::logic_error("Hamming distance failure");
  }

  murk::data ctext =
      murk::in<std::string>()
   >> murk::web::http::navigate
   >> murk::web::http::get
   >> murk::base64_decode
   << "https://cryptopals.com/static/challenge-data/6.txt"s;

  auto expected = murk::crypto::dist_conv(murk::crypto::twist_char_dist);

  auto key = murk::crypto::xor_vigenere::crack(expected, ctext);

  for (size_t i = 2; i < 40; ++i) {
    auto key = murk::crypto::xor_vigenere::crack_with_known_len(expected, ctext, i);
    if (auto iter = std::find_if_not(key.begin(), key.end(), [](auto i) { return i == 0; });
        iter != key.end())
      std::cout << murk::deserialise<std::string>(murk::crypto::xor_vigenere::decrypt(key, ctext)) << std::endl;
  }


//  std::cout << murk::deserialise<std::string>(murk::crypto::xor_vigenere::decrypt(key, ctext)) << std::endl;

  return 0;
}
