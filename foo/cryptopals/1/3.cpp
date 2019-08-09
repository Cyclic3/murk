#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>
#include <murk/flows/seq.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/crypto/analysis.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  auto ctext = murk::hex_decode << "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"s;

  auto decrypt =
      [](uint8_t key) { return [=](uint8_t i) { return i ^ key; };  }
   >> (murk::translate_span<uint8_t, char>> ctext);

  auto score_key =
      decrypt
   >> murk::cast_span<char, murk::crypto::token_t>
   >> murk::count<murk::crypto::token_t>
   >> murk::crypto::normalise_freq
   >> (murk::crypto::score_dist_match < (murk::crypto::dist_conv << murk::crypto::twist_char_dist));

  uint8_t best_key = '\0';
  murk::crypto::freq_t best_score = std::numeric_limits<murk::crypto::freq_t>::infinity();

  for (size_t i = 0; i < 256; ++i) {
    auto score = score_key(i);
    if (score < best_score) {
      best_key = i;
      best_score = score;
    }
  }

  auto a = decrypt(best_key);

  murk::alert("{}", std::string{a.begin(), a.end()});

  return 0;
}
