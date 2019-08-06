#include "murk/crypto/analysis.hpp"

namespace murk::crypto {
  std::array<char, 26> english_lcase = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z'
  };

  std::array<char, 26> english_ucase {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z'
  };

  std::array<char, 10> arabic_numerals {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
  };

  // TODO: use uppercase
  std::map<char, float> english_dist {
    { 'e', 0.12702 },
    { 't', 0.09056 },
    { 'a', 0.08167 },
    { 'o', 0.07507 },
    { 'i', 0.06966 },
    { 'n', 0.06749 },
    { 's', 0.06327 },
    { 'h', 0.06094 },
    { 'r', 0.05987 },
    { 'd', 0.04253 },
    { 'l', 0.04025 },
    { 'c', 0.02782 },
    { 'u', 0.02758 },
    { 'm', 0.02406 },
    { 'w', 0.0236  },
    { 'f', 0.02228 },
    { 'g', 0.02015 },
    { 'y', 0.01974 },
    { 'p', 0.01929 },
    { 'b', 0.01492 },
    { 'v', 0.00978 },
    { 'k', 0.00772 },
    { 'j', 0.00153 },
    { 'x', 0.0015  },
    { 'q', 0.00095 },
    { 'z', 0.00074 },
  };

  std::map<size_t, size_t> match_distribution(std::map<size_t, float> dist, std::map<size_t, float> msg) {
    // TODO: actual stats, as opposed to bs matching
    // maybe even phrase based stuff. We'll see.
    //
    // TODO: generate candidates, in order of chance.
    // We actually kind of need this
    if (dist.size() != msg.size())
      throw std::invalid_argument("Cannot match distributions of different size");

    std::map<size_t, size_t> ret;

    // Sorry
    for (auto dist_iter = dist.begin(), msg_iter = msg.begin();
         dist_iter != dist.end();
         ++dist_iter, ++msg_iter) {
      ret[msg_iter->first] = dist_iter->second;
    }

    return ret;
  }

  namespace distributions {

    std::vector<std::pair<std::pair<token_t, token_t>, float>> calc_pair_distribution(gsl::span<const token_t> i);

    std::map<token_t, std::map<token_t, float>> calc_next_distribution(gsl::span<const size_t> i);
  }
}
