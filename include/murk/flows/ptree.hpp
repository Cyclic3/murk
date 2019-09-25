#pragma once

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace murk {
  namespace pt = boost::property_tree;
  using ptree = pt::ptree;

  ptree parse_json(std::string s);
  std::string render_json(ptree pt);

  ptree parse_xml(std::string s);
  std::string render_xml(ptree pt);
}
