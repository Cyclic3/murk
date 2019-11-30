#pragma once

#include <gsl/gsl-lite.hpp>

#include <capstone/capstone.h>

namespace murk::pwn {
  [[noreturn]]
  inline void throw_cs_err(::cs_err err) {
    const char* str = ::cs_strerror(err);
    auto str_f = gsl::finally([&](){delete(str);});
    throw std::runtime_error(str);
  }

  inline void check_cs_err(::cs_err err) {
    if (err != ::CS_ERR_OK)
      throw_cs_err(err);
  }
}
