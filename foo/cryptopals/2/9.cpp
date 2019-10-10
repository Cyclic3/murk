#include <murk/common.hpp>

#include <murk/crypto/padding.hpp>

int main() {
  auto in = murk::serialise(u8"YELLOW SUBMARINE");
  auto expected = murk::serialise(u8"YELLOW SUBMARINE\4\4\4\4");
  murk::log("Padding working: {}", murk::crypto::pkcs7_add(20, in) == expected);
}
