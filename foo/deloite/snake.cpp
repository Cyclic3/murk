#include <murk/crypto/awful.hpp>

#include <murk/flows/fs.hpp>
#include <murk/common.hpp>

int main() {
  auto b = murk::fs::read_all_bytes("/tmp/a.png");
  murk::log("{}", murk::deserialise<std::string>(murk::crypto::xor_single::crypt(b, 0x57)));
}
