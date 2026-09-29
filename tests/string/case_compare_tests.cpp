#include <RST/RST.hpp>

#include <map>
#include <string>

namespace Str = RST::String;

// The character helpers are constexpr and locale independent.
static_assert(Str::isSpace(' ') && Str::isSpace('\t') && Str::isSpace('\v') && !Str::isSpace('a'));
static_assert(Str::isDigit('7') && !Str::isDigit('x'));
static_assert(Str::isHexDigit('f') && Str::isHexDigit('B') && !Str::isHexDigit('g'));
static_assert(Str::toLower('Q') == 'q' && Str::toUpper('q') == 'Q' && Str::toLower('1') == '1');
static_assert(Str::equalsIgnoreCase("Content-Type", "content-TYPE"));
static_assert(Str::findIgnoreCase("Hello World", "WORLD") == 6);

// Bytes above 0x7F (UTF-8) were undefined behaviour with ::tolower on a signed char: now untouched.
TEST_CASE(test_case_conversion_keeps_utf8_bytes) {
    std::string text = "Élan VITAL ÿ";
    Str::toLower(text);
    CHECK(text == "Élan vital ÿ");

    Str::toUpper(text);
    CHECK(text == "ÉLAN VITAL ÿ");

    int changed = 0;
    for (int c = -128; c < 0; ++c) {
        const char ch = static_cast<char>(c);
        changed += (Str::toLower(ch) != ch || Str::toUpper(ch) != ch) ? 1 : 0;
    }
    CHECK(changed == 0);
}

// The copies accept any string-like argument.
TEST_CASE(test_case_copies) {
    const std::string_view view = "MiXeD 123";
    CHECK(Str::toLowerCopy(view) == "mixed 123");
    CHECK(Str::toUpperCopy("MiXeD 123") == "MIXED 123");
    CHECK(Str::toLowerCopy("") == "");
}

TEST_CASE(test_ignore_case_prefix_suffix_and_search) {
    CHECK(Str::startsWithIgnoreCase("HTTP/1.1 200", "http/"));
    CHECK(!Str::startsWithIgnoreCase("HT", "http"));
    CHECK(Str::endsWithIgnoreCase("photo.JPG", ".jpg"));
    CHECK(!Str::endsWithIgnoreCase("jpg", ".jpg"));
    CHECK(Str::containsIgnoreCase("The Quick Brown Fox", "quick brown"));
    CHECK(!Str::containsIgnoreCase("The Quick Brown Fox", "slow"));
    CHECK(Str::findIgnoreCase("aAaA", "AA", 1) == 1u);
    CHECK(Str::findIgnoreCase("abc", "", 3) == 3u);
    CHECK(Str::findIgnoreCase("abc", "x", 99) == std::string_view::npos);
    CHECK(Str::contains("key=value", '='));
    CHECK(Str::contains("key=value", "=val"));
}

TEST_CASE(test_compare_ignore_case_orders_like_compare) {
    CHECK(Str::compareIgnoreCase("apple", "APPLE") == 0);
    CHECK(Str::compareIgnoreCase("apple", "Banana") < 0);
    CHECK(Str::compareIgnoreCase("Cherry", "banana") > 0);
    CHECK(Str::compareIgnoreCase("app", "APPLE") < 0);
    CHECK(Str::compareIgnoreCase("apple", "APP") > 0);
}
