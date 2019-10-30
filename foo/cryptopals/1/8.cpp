#include <murk/crypto/block.hpp>
#include <murk/flows/fs.hpp>
#include <murk/ext/manager.hpp>

#include <murk/common.hpp>

int main() {
  auto ctexts = murk::fs::read_all_lines(murk::ext::mgr.obtain("https://cryptopals.com/static/challenge-data/8.txt"));
  for (auto i : ctexts)
    if (murk::crypto::ecb_detect(murk::hex_decode(i), 16))
      murk::log("{}", i);

}
