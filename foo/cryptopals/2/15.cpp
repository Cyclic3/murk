#include <murk/crypto/block.hpp>

#include <murk/common.hpp>

int main() {
  auto a = "ICE ICE BABY\x04\x04\x04\x04"_b;
  if (!murk::crypto::pkcs7_validate(a))
    throw std::logic_error("False negative");
  auto b = "ICE ICE BABY\x05\x05\x05\x05"_b;
  if (murk::crypto::pkcs7_validate(b))
    throw std::logic_error("False positive");
  auto c = "ICE ICE BABY\x01\x02\x03\x04"_b;
  if (murk::crypto::pkcs7_validate(c))
    throw std::logic_error("False positive");
}
