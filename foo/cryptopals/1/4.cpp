#include <murk/flows/fs.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/ext/manager.hpp>

#include <murk/common.hpp>

int main() {
  auto f = murk::ext::mgr.obtain("https://cryptopals.com/static/challenge-data/4.txt");
  auto lines = murk::map(murk::fs::read_all_lines(f), std::function{murk::hex_decode});

  murk::flow_t<murk::data, double> score = murk::crypto::xor_single::score > murk::crypto::dist_conv(murk::crypto::twist_char_dist);

  auto res = murk::maximum(lines, score).first;

  auto k =  murk::crypto::xor_single::crack(res, murk::crypto::dist_conv(murk::crypto::twist_char_dist));
  auto plain = murk::crypto::xor_single::crypt(res, k);
  murk::log("{}", murk::deserialise<std::string>(plain));
}

