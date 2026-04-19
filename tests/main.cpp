#include <RST/RST.hpp>

#include <memory>
#include <iostream>
#include <vector>

int add(int a, int b) {return a + b;}


TEST_CASE(simple_test) {
  CHECK(3 == 3);
  CHECK(add(4, 5) == 9);
};

int main(int argc, const char** argv) {
    // Example: Using the Logger and Parser

    RST::Log::Logger logger("TestLogger");

    logger.addSink(std::make_shared<RST::Log::ConsoleSink>());

    logger.info("Application started.");

    RST::Parser::ArgParser parser;
    parser.addFlag({"--help", "-h"}, "Help message");

    try {
        parser.parse(argc, argv);
    } catch (const RST::Parser::HelpRequested& h) {
        parser.printHelp();
        return 0;
    } catch (const std::exception& e) {
        logger.error(e.what());
        parser.printHelp();
        return 1;
    }

    // Example: Using String utilities
    std::string text = "   Hello RSTools!   ";
    std::cout << RST::String::LtrimCopy(text) << "\n";

    logger.log(RST::Log::LogLevel::Info, "Application stop.");
    return RUN_ALL_TESTS();
}
