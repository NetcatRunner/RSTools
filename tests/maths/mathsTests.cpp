#include <RST/RST.hpp>

int add(int a, int b) {return a + b;}

TEST_CASE(simple_test) {
  CHECK(3 == 3);
  CHECK(add(4, 5) == 9);
};

TEST_CASE(simple_vector_test) {
  RST::Maths::Vec2i v(1,3);

  CHECK(v.getX() == 1);
  CHECK(v.getY() == 3);

  v += RST::Maths::Vec2i(5, 1);

  CHECK(v == RST::Maths::Vec2i(6, 4));
};
