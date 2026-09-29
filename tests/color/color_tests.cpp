#include <RST/RST.hpp>

#include <cmath>
#include <limits>
#include <sstream>

using RST::Color::Color;
namespace Colors = RST::Color;

static_assert(sizeof(Color) == 4);
static_assert(Color(0x11223344u).toHex() == 0x11223344u);
static_assert(Color::fromRgb(0xFF8000) == Color(255, 128, 0, 255));
static_assert(Color::fromHexString("#F80") == Color(255, 136, 0));
static_assert(!Color::fromHexString("#12345").has_value());
static_assert(Colors::lerp(Colors::Black, Colors::White, 0.5f) == Color(128, 128, 128));

TEST_CASE(test_color_hex_conversions) {
    const Color color(0x12, 0x34, 0x56, 0x78);
    CHECK(color.toHex() == 0x12345678u);
    CHECK(color.toRgb() == 0x123456u);
    CHECK(Color(color.toHex()) == color);
    CHECK(Color::fromRgb(0x123456, 0x78) == color);
    CHECK(color.toHexString() == "#12345678");
    CHECK(color.toHexString(false) == "#123456");

    std::ostringstream out;
    out << Colors::Orange;
    CHECK(out.str() == "#FFA500FF");
}

TEST_CASE(test_color_from_hex_string) {
    CHECK(Color::fromHexString("#ff8000") == Color(255, 128, 0));
    CHECK(Color::fromHexString("FF800080") == Color(255, 128, 0, 128));
    CHECK(Color::fromHexString("#abc") == Color(0xAA, 0xBB, 0xCC));
    CHECK(Color::fromHexString("#abcd") == Color(0xAA, 0xBB, 0xCC, 0xDD));
    CHECK(!Color::fromHexString("").has_value());
    CHECK(!Color::fromHexString("#").has_value());
    CHECK(!Color::fromHexString("#GG0000").has_value());
    CHECK(!Color::fromHexString("#123456789").has_value());
    CHECK(!Color::fromHexString("##123456").has_value());
}

// lerp clamps t and rounds: t outside [0, 1] was an out-of-range float to uint8 conversion (UB).
TEST_CASE(test_color_lerp_clamps_and_rounds) {
    const Color from(0, 100, 255, 0);
    const Color to(255, 200, 0, 255);
    CHECK(Colors::lerp(from, to, 0.f) == from);
    CHECK(Colors::lerp(from, to, 1.f) == to);
    CHECK(Colors::lerp(from, to, 2.f) == to);
    CHECK(Colors::lerp(from, to, -1.f) == from);
    CHECK(Colors::lerp(from, to, std::numeric_limits<float>::quiet_NaN()) == from);
    CHECK(Colors::lerp(Colors::White, Colors::Black, 0.5f) == Color(128, 128, 128));
}

TEST_CASE(test_color_float_conversions) {
    CHECK(Color::fromFloat(1.f, 0.5f, 0.f) == Color(255, 128, 0));
    CHECK(Color::fromFloat(2.f, -1.f, std::numeric_limits<float>::quiet_NaN(), 0.f) == Color(255, 0, 0, 0));

    const auto [r, g, b, a] = Color(255, 0, 51, 255).toFloat();
    CHECK(r == 1.f);
    CHECK(g == 0.f);
    CHECK(RST::Maths::isNearlyEqual(b, 0.2f));
    CHECK(a == 1.f);
}

TEST_CASE(test_color_hsv) {
    const RST::Color::Hsv red = Colors::Red.toHsv();
    CHECK(red.h == 0.f);
    CHECK(red.s == 1.f);
    CHECK(red.v == 1.f);
    CHECK(Colors::Magenta.toHsv().h == 300.f);
    CHECK(Colors::Gray.toHsv().s == 0.f);

    CHECK(Color::fromHsv({120.f, 1.f, 1.f}) == Colors::Green);
    CHECK(Color::fromHsv({240.f, 1.f, 1.f, 0.5f}) == Color(0, 0, 255, 128));
    CHECK(Color::fromHsv({-120.f, 1.f, 1.f}) == Colors::Blue);
    CHECK(Color::fromHsv({720.f, 1.f, 1.f}) == Colors::Red);
    CHECK(Color::fromHsv({360.f, 1.f, 1.f}) == Colors::Red);
    CHECK(Color::fromHsv({std::numeric_limits<float>::quiet_NaN(), 0.f, 0.5f}) == Color(128, 128, 128));
}

// Every 8-bit color survives a trip through HSV.
TEST_CASE(test_color_hsv_round_trip) {
    int mismatches = 0;
    for (int r = 0; r < 256; r += 5) {
        for (int g = 0; g < 256; g += 7) {
            for (int b = 0; b < 256; b += 3) {
                const Color color(static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b));
                mismatches += Color::fromHsv(color.toHsv()) != color ? 1 : 0;
            }
        }
    }
    CHECK(mismatches == 0);
}

TEST_CASE(test_color_variations) {
    CHECK(Colors::Red.withAlpha(10) == Color(255, 0, 0, 10));
    CHECK(Color(10, 20, 30, 40).inverted() == Color(245, 235, 225, 40));
    CHECK(Colors::White.grayscale() == Colors::White);
    CHECK(Colors::Black.grayscale() == Colors::Black);
    CHECK(Colors::Green.grayscale() == Color(182, 182, 182));
}

TEST_CASE(test_color_blend) {
    CHECK(Colors::blend(Colors::Red, Colors::Blue) == Colors::Red);
    CHECK(Colors::blend(Colors::Transparent, Colors::Blue) == Colors::Blue);
    CHECK(Colors::blend(Colors::Red.withAlpha(128), Colors::Blue) == Color(128, 0, 127, 255));
    CHECK(Colors::blend(Colors::Red.withAlpha(128), Colors::Transparent) == Colors::Red.withAlpha(128));
    CHECK(Colors::blend(Colors::White.withAlpha(128), Colors::Black.withAlpha(128)) == Color(170, 170, 170, 192));
}
