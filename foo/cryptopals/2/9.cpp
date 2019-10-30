#include <murk/common.hpp>

#include <murk/crypto/padding.hpp>

int main() {
  auto in = murk::serialise("YELLOW SUBMARINE");
  auto expected = murk::serialise("YELLOW SUBMARINE\4\4\4\4");
  murk::log("Padding working: {}", murk::crypto::pkcs7_add(in, 20) == expected);
}
