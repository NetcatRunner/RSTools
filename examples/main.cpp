#include <RST/RST.hpp>

#include <memory>
#include <iostream>

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
}
