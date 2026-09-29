#include <RST/RST.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

namespace Str = RST::String;
using namespace RST::String::Literals;

static_assert(Str::hash("") == 0xcbf29ce484222325ull);
static_assert(Str::hash("a") == 0xaf63dc4c8601ec8cull);  // FNV-1a 64 reference value
static_assert("start"_hash == Str::hash("start"));
static_assert("start"_hash != "stop"_hash);

TEST_CASE(test_replace_all) {
    std::string text = "a-b-c";
    CHECK(Str::replaceAll(text, "-", " - ") == 2u);
    CHECK(text == "a - b - c");

    std::string shrink = "aaaa";
    CHECK(Str::replaceAll(shrink, "aa", "b") == 2u);
    CHECK(shrink == "bb");

    std::string none = "abc";
    CHECK(Str::replaceAll(none, "x", "y") == 0u);
    CHECK(Str::replaceAll(none, "", "y") == 0u);
    CHECK(none == "abc");

    std::string erase = "a, b, c";
    CHECK(Str::replaceAll(erase, ", ", "") == 2u);
    CHECK(erase == "abc");
}

// The replacement text is not scanned again: no infinite growth when 'to' contains 'from'.
TEST_CASE(test_replace_all_does_not_rescan) {
    std::string text = "x";
    CHECK(Str::replaceAll(text, "x", "xx") == 1u);
    CHECK(text == "xx");
}

// 'from' and 'to' may point into the string being modified.
TEST_CASE(test_replace_all_with_aliasing_arguments) {
    std::string text = "ab ab";
    const std::string_view from(text.data(), 2);
    const std::string_view to(text.data() + 1, 1);
    CHECK(Str::replaceAll(text, from, to) == 2u);
    CHECK(text == "b b");
}

TEST_CASE(test_replace_first_and_copy) {
    std::string text = "one one one";
    CHECK(Str::replaceFirst(text, "one", "1"));
    CHECK(text == "1 one one");
    CHECK(!Str::replaceFirst(text, "two", "2"));

    CHECK(Str::replaceAllCopy("1.2.3", ".", "::") == "1::2::3");
    CHECK(Str::replaceAllCopy("abc", "z", "y") == "abc");
}

TEST_CASE(test_parse_integers) {
    CHECK(Str::parseNumber<int>("42") == 42);
    CHECK(Str::parseNumber<int>("  -17\n") == -17);
    CHECK(Str::parseNumber<int>("+8") == 8);
    CHECK(Str::parseNumber<unsigned>("0xff", 16) == 255u);
    CHECK(Str::parseNumber<unsigned>("+0XFF", 16) == 255u);
    CHECK(Str::parseNumber<unsigned>("FF", 16) == 255u);
    CHECK(Str::parseNumber<int>("101", 2) == 5);
    CHECK(Str::parseNumber<int>("12", 8) == 10);
    CHECK(Str::parseNumber<int>("-z", 36) == -35);
    CHECK(Str::parseNumber<std::int64_t>("9223372036854775807") == std::numeric_limits<std::int64_t>::max());
}

// Trailing garbage, overflow, a sign on an unsigned type or a bad base all fail instead of guessing.
TEST_CASE(test_parse_integers_rejects_invalid_text) {
    CHECK(!Str::parseNumber<int>("42px").has_value());
    CHECK(!Str::parseNumber<int>("").has_value());
    CHECK(!Str::parseNumber<int>("   ").has_value());
    CHECK(!Str::parseNumber<int>("4 2").has_value());
    CHECK(!Str::parseNumber<int>("+-3").has_value());
    CHECK(!Str::parseNumber<int>("++3").has_value());
    CHECK(!Str::parseNumber<std::uint8_t>("256").has_value());
    CHECK(!Str::parseNumber<unsigned>("-1").has_value());
    CHECK(!Str::parseNumber<int>("12", 1).has_value());
    CHECK(!Str::parseNumber<int>("12", 37).has_value());
    CHECK(!Str::parseNumber<int>("0x-5", 16).has_value());
    CHECK(!Str::parseNumber<int>("0x", 16).has_value());
}

TEST_CASE(test_parse_floating_point) {
    CHECK(Str::parseNumber<double>("3.5") == 3.5);
    CHECK(Str::parseNumber<double>(" -0.25 ") == -0.25);
    CHECK(Str::parseNumber<double>("1e3") == 1000.0);
    CHECK(Str::parseNumber<float>("-.5") == -0.5f);
    CHECK(Str::parseNumber<float>("+.5") == 0.5f);
    CHECK(!Str::parseNumber<double>("1.5", 16).has_value());
    CHECK(!Str::parseNumber<double>("1.5.2").has_value());
    CHECK(!Str::parseNumber<double>("abc").has_value());
    CHECK(!Str::parseNumber<float>("1e999").has_value());
}

static_assert(Str::parseBool("yes") == true);

TEST_CASE(test_parse_bool) {
    CHECK(Str::parseBool("true") == true);
    CHECK(Str::parseBool(" YES ") == true);
    CHECK(Str::parseBool("on") == true);
    CHECK(Str::parseBool("1") == true);
    CHECK(Str::parseBool("False") == false);
    CHECK(Str::parseBool("off") == false);
    CHECK(Str::parseBool("0") == false);
    CHECK(!Str::parseBool("maybe").has_value());
    CHECK(!Str::parseBool("").has_value());
}

// A switch over strings through their hash.
TEST_CASE(test_hash_in_switch) {
    const auto dispatch = [](std::string_view command) {
        switch (Str::hash(command)) {
            case "start"_hash: return 1;
            case "stop"_hash:  return 2;
            default:           return 0;
        }
    };
    CHECK(dispatch("start") == 1);
    CHECK(dispatch(std::string("stop")) == 2);
    CHECK(dispatch("restart") == 0);
}
