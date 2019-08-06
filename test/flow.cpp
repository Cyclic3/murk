#include <gtest/gtest.h>
#include "murk/flow.hpp"

using namespace murk::flow_ops;


murk::flow_t<char, int> char2int = [](char i) -> int { return i; };
murk::flow_t<int, float> int2float = [](int i) -> float { return i; };
murk::flow_t<long, float> long2float = [](int i) -> float { return i; };

TEST(flow, casts) {
  auto flow = char2int >> int2float;
  murk::flow_t<char, float> flow_td = char2int >> int2float;

  ASSERT_EQ(flow('\1'), 1.);
  ASSERT_EQ(flow_td('\1'), 1.);
}

TEST(flow, implicitCasts) {
  auto flow = char2int >> long2float;

  ASSERT_EQ(flow('\1'), 1.);
}

void foo(int, int);

TEST(flow, arithmetic) {
  auto flow =
      (std::plus<int>() | 5) >> (std::minus<int>() | 2) >> (std::minus<int>() & 3);

  int f = flow(3);

  ASSERT_EQ(f, -9);
}
