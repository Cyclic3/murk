#include "murk/baby.hpp"

#include <string>

#include "murk/data.hpp"

#include "murk/flows/bytes.hpp"

#include "murk/filt/zealous.hpp"

#include "murk/priv/docker.hpp"

#include "murk/plod/png_chunks.hpp"

#include "murk/web/uri.hpp"
#include "murk/web/ssti.hpp"

namespace murk::baby {
  std::string base64_encode(std::string s) { return murk::base64_encode(murk::get_data_const_ref(s)); }
  std::string base64_decode(std::string s) { return murk::deserialise<std::string>(murk::base64_decode(s)); }

  std::string zealous_escape_c(std::string s) { return murk::filt::zealous_escape_c(s); }
  std::string zealous_urlencode(std::string s) { return murk::filt::zealous_urlencode(s); }

  std::string hex_encode(std::string s) { return murk::hex_encode(murk::get_data_const_ref(s)); }
  std::string hex_decode(std::string s) { return murk::deserialise<std::string>(murk::hex_decode(s)); }

  std::string priv_docker(std::string prog) { return murk::priv::docker(prog); }

  std::string png_chunks(std::string plod) {
    murk::plod::png_chunks chunks;
    chunks.header.width = chunks.header.height = 1;
    chunks.chunks.emplace_back(murk::plod::png_chunks::chunk{.dat = murk::serialise(plod)});
    chunks.chunks.emplace_back(murk::plod::png_chunks::chunk{.type = {'I', 'D', 'A', 'T'}, .dat = {0x08, 0xd7, 0x63, 0x68, 0x00, 0x00, 0x00, 0x82, 0x00, 0x81}});
    return murk::base64_encode(chunks.render());
  }

  std::string ssti_jinja2(std::string plod) { return murk::web::jinja2(plod); }

  std::string url_encode(std::string s) { return murk::web::uri::encode(s); }
  std::string url_decode(std::string s) { return murk::web::uri::decode(s); }

  std::string get_random_bytes(size_t count) {
    auto fd = ::open("/dev/urandom", 0);
    if (fd < 0)
      throw std::runtime_error("Could not open urandom");
    murk::data ret(count);
    ::read(fd, ret.data(), count);
    ::close(fd);
    return murk::hex_encode(ret);
  }
}
