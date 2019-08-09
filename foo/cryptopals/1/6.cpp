#include <murk/flow.hpp>
#include <murk/flows/bytes.hpp>
#include <murk/flows/string.hpp>
#include <murk/flows/web.hpp>
#include <murk/crypto/analysis.hpp>

using namespace murk::flow_ops;
using namespace std::string_literals;

int main() {
  {
    std::string a = "this is a test";
    std::string b = "wokka wokka!!!";

    size_t d = murk::crypto::hamming_distance(murk::get_data_const_ref(a), murk::get_data_const_ref(b));

    if (d != 37)
      throw std::logic_error("Hamming distance failure");
  }

  murk::data ctext =
      murk::in<std::string>()
   >> murk::web::http::navigate
   >> murk::web::http::get
   >> (murk::replace < std::regex{" "} < " "s)
   >> murk::base64_decode
   << "https://cryptopals.com/static/challenge-data/6.txt"s;

  return 0;
}
