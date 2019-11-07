#include <murk/crypto/awful.hpp>

#include <murk/common.hpp>

int main() {
  auto b = "535646564e014502045c4c6d055600474b6b025e015c0603055c014a"_hex;

  std::array<uint8_t, 28> k = {0};

  for (auto i = 0; i < 256; ++i) {
    auto s = murk::deserialise<std::string>(murk::crypto::xor_single::crypt(b, i));
    if (s[0] == 'd')
      k[0] = i;
    if (s[1] == 'C')
      k[1] = i;
    if (s[2] == 'T')
      k[2] = i;
    if (s[3] == 'F')
      k[3] = i;
    if (s[4] == '{')
      k[4] = i;
    if (s[27] == '}')
      k[27] = i;
  }

  murk::log("{}", murk::deserialise<std::string>(murk::crypto::xor_vigenere::crypt(b, murk::data_const_ref{k}.subspan(0, 9))));
}
