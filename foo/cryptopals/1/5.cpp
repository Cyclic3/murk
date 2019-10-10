#include <murk/crypto/awful.hpp>

#include <murk/common.hpp>

int main() {
  auto key = u8"ICE";
  auto msg = u8"Burning 'em, if you ain't quick and nimble";
  auto ctext = murk::crypto::xor_vigenere::encrypt(murk::serialise(key), murk::serialise(msg));

  murk::log("{}", murk::hex_encode(ctext));
}
