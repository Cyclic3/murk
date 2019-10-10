#include <murk/flows/fs.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/ext/manager.hpp>

#include <murk/common.hpp>

int main() {
  auto ctext = murk::base64_decode(murk::fs::read_all_text(murk::ext::mgr.obtain("https://cryptopals.com/static/challenge-data/6.txt")));
  auto key = murk::crypto::xor_vigenere::crack(murk::crypto::dist_conv(murk::crypto::twist_char_dist), ctext);
  auto ptext = murk::crypto::xor_vigenere::decrypt(key, ctext);

  murk::log("{}", murk::deserialise<std::string>(ptext));
}
