#pragma once

#include "murk/data.hpp"

#include <botan/crc32.h>

namespace murk::plod {
  class png_chunks {
  public:
    struct chunk {
      std::array<char, 4> type;
      data dat;

      template<typename Iter>
      void render_into(Iter iter) const {
        Botan::CRC32 crc32;
        crc32.update(reinterpret_cast<const uint8_t*>(type.data()), type.size());
        crc32.update(dat);
        std::array<uint8_t, 32 / 8> csum;
        crc32.final(csum.data());

        to_big_endian(static_cast<uint32_t>(dat.size()), iter);
        std::copy(type.begin(), type.end(), iter);
        std::copy(dat.begin(), dat.end(), iter);
        std::copy(csum.begin(), csum.end(), iter);
      }
    };
    enum class colour_type : uint8_t {
      Greyscale = 0,
      Rgb = 2,
      Indexed = 3,
      GreyscaleAlpha = 4,
      Rgba = 6,
    };
    enum class comp_method {
      Deflate = 0,
    };
    enum class filter_method {
      Adaptive = 0,
    };
    enum class interlace_method {
      None = 0,
      Adam7 = 1,
    };

    struct ihdr {
      uint32_t width;
      uint32_t height;
      uint8_t depth;
      colour_type colour;
      comp_method comp;
      filter_method filter;
      interlace_method interlace;
      operator chunk() const;
    };
    struct iend {
      operator chunk() const;
    };

    struct plte;

  public:
    ihdr header;
    std::vector<chunk> chunks;

  public:
    data render() const;
  };
}
