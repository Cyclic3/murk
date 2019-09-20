#include "murk/plod/png_chunks.hpp"

namespace murk::plod {
  png_chunks::ihdr::operator chunk() const {
    chunk ret;
    ret.type = { 'I', 'H', 'D', 'R' };


    {
      auto iter = std::back_inserter(ret.dat);
      to_big_endian(width, iter);
      to_big_endian(height, iter);
    }

    ret.dat.push_back(static_cast<uint8_t>(depth));
    ret.dat.push_back(static_cast<uint8_t>(colour));
    ret.dat.push_back(static_cast<uint8_t>(comp));
    ret.dat.push_back(static_cast<uint8_t>(filter));
    ret.dat.push_back(static_cast<uint8_t>(interlace));

    return ret;
  }

  png_chunks::iend::operator chunk() const {
    chunk ret;
    ret.type = { 'I', 'E', 'N', 'D' };

    return ret;
  }

  data png_chunks::render() const {
    data ret;

    // Magic number
    ret.insert(ret.end(), {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A});

    auto iter = std::back_inserter(ret);

    chunk hdr = header;
    hdr.render_into(iter);

    for (auto& i : chunks)
      i.render_into(iter);

    chunk end = iend{};
    end.render_into(iter);

    return ret;
  }
}
