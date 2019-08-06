#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace murk {
  namespace pt = boost::property_tree;
  using ptree = pt::ptree;

  inline ptree parse_json(std::string s) {
    ptree ret;
    std::istringstream ss(s);
    pt::read_json(ss, ret);
    return ret;
  }
  inline ptree parse_xml(std::string s) {
    ptree ret;
    std::istringstream ss(s);
    pt::read_xml(ss, ret);
    return ret;
  }
  inline std::string render_json(ptree pt) {
    std::ostringstream ss;
    pt::write_json(ss, pt, false);
    // Boost adds a newline for some reason
    auto ret = ss.str();
    ret.pop_back();
    return ret;
  }
  inline std::string render_xml(ptree pt) {
    std::ostringstream ss;
    pt::write_xml(ss, pt);
    return ss.str();
  }
}
