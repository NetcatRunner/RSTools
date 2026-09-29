#include <RST/RST.hpp>

#include <string>

namespace Str = RST::String;

static_assert(Str::trimView("  \t hello \r\n") == "hello");
static_assert(Str::ltrimView("  hi  ") == "hi  ");
static_assert(Str::rtrimView("  hi  ") == "  hi");
static_assert(Str::trimView("--x--", "-") == "x");
static_assert(Str::trimView(" \t ").empty());

// Views do not allocate: the result points into the argument, even when everything is trimmed.
TEST_CASE(test_trim_view_points_into_source) {
    const std::string text = "   payload   ";
    const std::string_view trimmed = Str::trimView(text);
    CHECK(trimmed == "payload");
    CHECK(trimmed.data() == text.data() + 3);

    const std::string blank = "    ";
    CHECK(Str::ltrimView(blank).data() == blank.data() + blank.size());
    CHECK(Str::rtrimView(blank).data() == blank.data());
}

// All of std::isspace's characters are trimmed, \f and \v included.
TEST_CASE(test_trim_in_place) {
    std::string text = "\f\v \t value \n\r";
    Str::trim(text);
    CHECK(text == "value");

    std::string left = "  left  ";
    Str::ltrim(left);
    CHECK(left == "left  ");

    std::string right = "  right  ";
    Str::rtrim(right);
    CHECK(right == "  right");

    std::string blank = " \t\n ";
    Str::trim(blank);
    CHECK(blank.empty());

    std::string empty;
    Str::trim(empty);
    CHECK(empty.empty());
}

TEST_CASE(test_trim_custom_characters) {
    std::string path = "//usr/local//";
    Str::trim(path, "/");
    CHECK(path == "usr/local");

    CHECK(Str::trimCopy("0001200", "0") == "12");
    CHECK(Str::ltrimCopy("xxabcxx", "x") == "abcxx");
    CHECK(Str::rtrimCopy("xxabcxx", "x") == "xxabc");
    CHECK(Str::trimCopy("abc", "") == "abc");
}

TEST_CASE(test_trim_copies_leave_source_untouched) {
    const std::string source = "  both  ";
    CHECK(Str::trimCopy(source) == "both");
    CHECK(Str::ltrimCopy(source) == "both  ");
    CHECK(Str::rtrimCopy(source) == "  both");
    CHECK(source == "  both  ");
}
