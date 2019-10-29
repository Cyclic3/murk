#include <murk/flows/seq.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/crypto/analysis.hpp>

#include <murk/common.hpp>

int main() {
  auto in = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"_hex;
  auto k =  murk::crypto::xor_single::crack(in, murk::crypto::dist_conv(murk::crypto::twist_char_dist));
  auto res = murk::crypto::xor_single::crypt(in, k);
  murk::log("{}", murk::deserialise<std::string>(res));
}
