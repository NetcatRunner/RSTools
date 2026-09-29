#include <RST/string/String.hpp>

#include <iostream>
#include <string>
#include <vector>

int main() {
    namespace Str = RST::String;
    using namespace RST::String::Literals;

    // Trimming and case
    std::string name = "  Ada Lovelace \n";
    Str::trim(name);
    std::cout << Str::toUpperCopy(name) << '\n';                                  // ADA LOVELACE

    // Splitting and joining
    const std::vector<std::string> colors = Str::splitString("red, green,,blue", ", ");
    std::cout << Str::join(colors, " | ") << '\n';                                 // red | green | blue
    std::cout << Str::join({1, 2, 3}, "+") << '\n';                                // 1+2+3

    // Replacing and searching
    std::string path = "C:\\Users\\ada\\notes.TXT";
    Str::replaceAll(path, "\\", "/");
    if (Str::endsWithIgnoreCase(path, ".txt"))
        std::cout << path << " is a text file\n";

    // Numbers, without exceptions
    if (const auto port = Str::parseNumber<int>(" 8080 "))
        std::cout << "port " << *port << '\n';
    if (!Str::parseNumber<int>("80a"))
        std::cout << "80a is not a number\n";

    // A switch on strings
    switch (Str::hash("stop")) {
        case "start"_hash: std::cout << "starting\n"; break;
        case "stop"_hash:  std::cout << "stopping\n"; break;
        default:           std::cout << "unknown command\n"; break;
    }
    return 0;
}
