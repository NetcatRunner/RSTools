#include <RST/RST.hpp>

#include <cmath>
#include <limits>
#include <numbers>

using namespace RST::Maths;

static_assert(isNearlyEqual(toRad(180.0), std::numbers::pi));
static_assert(isNearlyEqual(toDeg(std::numbers::pi_v<float>), 180.f));
static_assert(Pi == std::numbers::pi && TwoPi == 2.0 * Pi);
static_assert(lerp(0.0, 10.0, 0.25) == 2.5);
static_assert(lerp(0.0, 10.0, 2.0) == 10.0);
static_assert(inverseLerp(10.0, 20.0, 15.0) == 0.5);
static_assert(smoothstep(0.0, 1.0, 0.5) == 0.5);
static_assert(clamp(5, 0, 3) == 3 && sign(-2.5) == -1 && RST::Maths::abs(-4) == 4);

TEST_CASE(test_smoothstep_edges) {
    CHECK(smoothstep(1.0, 2.0, 0.0) == 0.0);
    CHECK(smoothstep(1.0, 2.0, 3.0) == 1.0);
    CHECK(smoothstep(1.0, 1.0, 1.0) == 0.0);
    CHECK(inverseLerp(3.0, 3.0, 5.0) == 0.0);
}

TEST_CASE(test_wrap) {
    CHECK(wrap(370.0, 0.0, 360.0) == 10.0);
    CHECK(wrap(-30.0, 0.0, 360.0) == 330.0);
    CHECK(isNearlyEqual(wrap(3.5 * Pi, -Pi, Pi), -0.5 * Pi, 1e-12));
    CHECK(wrap(-1, 0, 5) == 4);
    CHECK(wrap(7, 0, 5) == 2);
    CHECK(wrap(-6, -2, 3) == -1);
    CHECK(wrap(5, 5, 10) == 5);
}

// The tolerance follows the magnitude: large neighbours compare equal, which an absolute epsilon
// (the previous behaviour) missed.
TEST_CASE(test_is_nearly_equal_scales_with_magnitude) {
    const float big = 1000.1f;
    const float next = std::nextafter(big, 2000.f);
    CHECK(isNearlyEqual(big, next));
    CHECK(!isNearlyEqual(1000.f, 1001.f));
    CHECK(isNearlyEqual(0.1f + 0.2f, 0.3f));
    CHECK(isNearlyEqual(1e-10f, 2e-10f));
    CHECK(!isNearlyEqual(1e-10, 2e-10));
    CHECK(isNearlyEqual(3, 3));
    CHECK(!isNearlyEqual(3, 4));
}

TEST_CASE(test_is_nearly_equal_special_values) {
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    CHECK(isNearlyEqual(inf, inf));
    CHECK(!isNearlyEqual(inf, 1.0));
    CHECK(!isNearlyEqual(inf, -inf));
    CHECK(!isNearlyEqual(nan, nan));
    CHECK(!isNearlyEqual(nan, 0.0));
}
