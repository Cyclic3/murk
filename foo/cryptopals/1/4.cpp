#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>
#include <murk/flows/seq.hpp>
#include <murk/flows/string.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/crypto/analysis.hpp>
#include <murk/flows/web.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {

  // First find the one we want
  murk::data best_ctext;
  {
    std::vector<murk::data> ctexts =
        murk::in<std::string>()
     >> murk::web::uri::parse
     >> murk::web::http::navigate
     >> murk::web::http::get
     >> (murk::split < "\n"s)
     >> (std::function(murk::translate_span<std::string, murk::data>) < murk::hex_decode)
     << "https://cryptopals.com/static/challenge-data/4.txt";

    murk::flow_t<murk::data_const_ref, double> score_ctext =
        murk::in<murk::data_const_ref>()
     >> murk::cast_span<uint8_t, murk::crypto::token_t>
     >> murk::count<murk::crypto::token_t>
     >> murk::crypto::normalise_freq
     >> (murk::crypto::score_dist_compare < (murk::crypto::dist_conv << murk::crypto::twist_char_dist));

    murk::crypto::freq_t best_score = std::numeric_limits<murk::crypto::freq_t>::infinity();

    for (auto& ctext : ctexts) {
      auto score = score_ctext(ctext);
      if (score < best_score) {
        best_ctext = ctext;
        best_score = score;
      }
    }
  }

  // Then reuse previous stats
  auto& ctext = best_ctext;

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
}

