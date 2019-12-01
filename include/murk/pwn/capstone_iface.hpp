#pragma once

#include <capstone/capstone.h>

#include <cppthings/defer.hpp>

namespace murk::pwn {
  [[noreturn]]
  inline void throw_cs_err(::cs_err err) {
    const char* str = ::cs_strerror(err);
    auto str_f = cppthings::defer([&](){delete(str);});
    throw std::runtime_error(str);
  }

  inline void check_cs_err(::cs_err err) {
    if (err != ::CS_ERR_OK)
      throw_cs_err(err);
  }
}
