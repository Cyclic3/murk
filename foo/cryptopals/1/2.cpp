#include <murk/common.hpp>

int main() {
  murk::log("{}", murk::hex_encode(murk::xor_bytes("1c0111001f010100061a024b53535009181c"_hex, "686974207468652062756c6c277320657965"_hex)));
}
