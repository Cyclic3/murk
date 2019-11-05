#include <murk/web/uri.hpp>

#include <murk/common.hpp>

int main() {
  std::string s = murk::web::uri::decode("%F6n2l%BEI%3C%F9Rs%85H%5C%40b37a05f04369c599a3c3cb01cdf26e45");

  auto orig = "Standard User";

  auto ptext = murk::serialise("Administrator");
  ptext.push_back('\0');

  for (size_t i = 0; i < ptext.size(); ++i) {
    s[i + 1] ^= orig[i] ^ ptext[i];
  }

  murk::alert("{}", murk::web::uri::encode(s));
}
