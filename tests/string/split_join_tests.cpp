#include <RST/RST.hpp>

#include <array>
#include <list>
#include <string>
#include <vector>

namespace Str = RST::String;

namespace {

    using Views = std::vector<std::string_view>;
    using Strings = std::vector<std::string>;

}

// Any of the delimiter characters splits, and empty parts are skipped.
TEST_CASE(test_split_string) {
    CHECK((Str::splitString("  hello   world ", " ") == Strings{"hello", "world"}));
    CHECK((Str::splitString("a,b;c", ",;") == Strings{"a", "b", "c"}));
    CHECK((Str::splitString(",,a,,b,,", ",") == Strings{"a", "b"}));
    CHECK((Str::splitString("abc", ",") == Strings{"abc"}));
    CHECK((Str::splitString("abc", "") == Strings{"abc"}));
    CHECK(Str::splitString("", ",").empty());
    CHECK(Str::splitString(",,,", ",").empty());
}

// The views point into the text: nothing is copied.
TEST_CASE(test_split_string_view) {
    const std::string text = "key=value";
    const Views parts = Str::splitStringView(text, "=");
    CHECK((parts == Views{"key", "value"}));
    CHECK(parts[0].data() == text.data());
    CHECK(parts[1].data() == text.data() + 4);

    CHECK((Str::splitStringView(" a\tb \n", " \t\n") == Views{"a", "b"}));
    CHECK(Str::splitStringView("", " ").empty());
}

// Quotes group words and disappear; "" is now an empty argument instead of nothing, as in a shell.
TEST_CASE(test_split_with_quotes) {
    CHECK((Str::splitStringWithQuotes("run \"my file.txt\" --fast", " ") == Strings{"run", "my file.txt", "--fast"}));
    CHECK((Str::splitStringWithQuotes("a \"\" b", " ") == Strings{"a", "", "b"}));
    CHECK((Str::splitStringWithQuotes("pre\"fix suf\"fix", " ") == Strings{"prefix suffix"}));
    CHECK((Str::splitStringWithQuotes("'a b' c", " ", '\'') == Strings{"a b", "c"}));
    CHECK(Str::splitStringWithQuotes("   ", " ").empty());
}

// join takes any range of string-like values.
TEST_CASE(test_join_any_range) {
    const std::vector<std::string> strings = {"a", "b", "c"};
    CHECK(Str::join(strings, ", ") == "a, b, c");

    const std::array<std::string_view, 3> views = {"x", "y", "z"};
    CHECK(Str::join(views, "") == "xyz");

    const std::list<const char*> pointers = {"1", "2"};
    CHECK(Str::join(pointers, "+") == "1+2");

    CHECK(Str::join(std::vector<std::string>{}, ",") == "");
    CHECK(Str::join({"usr", "local", "bin"}, "/") == "usr/local/bin");
    CHECK(Str::join({"only"}, ",") == "only");
    CHECK(Str::join(Str::splitStringView("a b  c", " "), "-") == "a-b-c");
}

// Numbers are written as text: an int used to be appended as the character of that code.
TEST_CASE(test_join_numbers) {
    CHECK(Str::join({4, 5, 6}, ", ") == "4, 5, 6");
    CHECK(Str::join(std::vector<int>{4, 5, 6}) == "456");
    CHECK(Str::join({1.5, 2.25, -3.0}, ";") == "1.5;2.25;-3");
    CHECK(Str::join({'a', 'b'}, "-") == "a-b");
}

TEST_CASE(test_join_string_legacy_behaviour) {
    const std::vector<std::string> parts = {"a", "b", "c"};
    CHECK(Str::joinString(parts, '-') == "a-b-c");
    CHECK(Str::joinString(parts) == "abc");
    CHECK(Str::joinString({}) == "");
}
