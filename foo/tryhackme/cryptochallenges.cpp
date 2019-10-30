#include <murk/crypto/awful.hpp>

#include <murk/ext/manager.hpp>

#include "murk/flows/fs.hpp"
#include "murk/flows/seq.hpp"
#include "murk/flows/bytes.hpp"
#include "murk/flow.hpp"

using namespace murk::flow_ops;
using namespace murk::lit_ops;
using namespace std::string_literals;

int main() {
  murk::alert("TASK1: {}", murk::base64_encode("49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d"_hex));


  murk::alert("TASK2: {}", murk::hex_encode(murk::xor_bytes("1c0111001f010100061a024b53535009181c"_hex, "686974207468652062756c6c277320657965"_hex)));

  {
    auto in = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"_hex;
    auto k =  murk::crypto::xor_single::crack(in, murk::crypto::dist_conv(murk::crypto::twist_char_dist));
    auto res = murk::crypto::xor_single::crypt(in, k);
    murk::alert("TASK3: {}", murk::deserialise<std::string>(res));
  }

  {
    auto f = murk::ext::mgr.obtain("https://cryptopals.com/static/challenge-data/4.txt");
    auto lines = murk::map(murk::fs::read_all_lines(f), std::function{murk::hex_decode});

    murk::flow_t<murk::data, double> score = murk::crypto::xor_single::score > murk::crypto::dist_conv(murk::crypto::twist_char_dist);

    auto res = murk::maximum(lines, score).first;

    auto k =  murk::crypto::xor_single::crack(res, murk::crypto::dist_conv(murk::crypto::twist_char_dist));
    auto plain = murk::crypto::xor_single::crypt(res, k);
    murk::alert("TASK4: {}", murk::deserialise<std::string>(plain));
  }

  {
    auto key = u8"ICE";
    auto msg = u8"Burning 'em, if you ain't quick and nimble";
    auto ctext = murk::crypto::xor_vigenere::crypt(murk::serialise(msg), murk::serialise(key));

    murk::alert("TASK5: {}", murk::hex_encode(ctext));
  }

  {

  }
}
