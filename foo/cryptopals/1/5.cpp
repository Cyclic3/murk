#include <murk/crypto/awful.hpp>

#include <murk/common.hpp>

int main() {
  auto key = "ICE";
  auto msg = "Burning 'em, if you ain't quick and nimble";
  auto ctext = murk::crypto::xor_vigenere::crypt(murk::serialise(msg), murk::serialise(key));

  murk::log("{}", murk::hex_encode(ctext));
}
