#include <RST/RST.hpp>

#include <memory>
#include <iostream>

int main(int argc, char** argv) {
    // Example: Using the Logger and Parser

    RST::Log::Logger logger("TestLogger");

    logger.addSink(std::make_shared<RST::Log::ConsoleSink>());

    logger.info("Application started.");

    RST::Parser::ArgParser parser("example", "Shows a few RSTools features.");
    parser.addFlag({"-v", "--verbose"}, "Print more details");
    parser.addOption({"-n", "--count"}, "How many greetings").integer().range(1, 10).defaultValue(1);

    // -h/--help and errors are handled here: nothing throws.
    if (auto result = parser.parse(argc, argv); !result) {
        result.print();
        return result.exitCode();
    }

    // Example: Using String utilities
    std::string text = "   Hello RSTools!   ";
    for (int i = 0; i < parser.get<int>("--count").value_or(1); ++i)
        std::cout << RST::String::ltrimCopy(text) << "\n";

    logger.log(RST::Log::LogLevel::Info, "Application stop.");
}
