#pragma once

//! Keeping this separate should massively improve compile times

#include "murk/web/http.hpp"

#include <boost/asio/ip/tcp.hpp>

namespace murk::web::http {
  struct remote::eps_t {
    boost::asio::ip::tcp::resolver::results_type val;
  };
}
