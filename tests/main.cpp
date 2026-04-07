#include <RST/RST.hpp>

#include <memory>
#include <iostream>

int main(int argc, const char** argv) {
    // Example: Using the Logger and Parser
    RST::Log::Log log("RST");

    log.AddSink(std::make_shared<RST::Log::ConsoleSink>());

    log.LogPrint(RST::Log::LogLevel::Info, "Application started.");

    RST::Parser::ArgParser parser;
    parser.addFlag({"--help", "-h"}, "Help message");

    parser.parse(argc, argv);
    if (parser.isSet("--help")) {
        std::cout << "Usage: ./app [--verbose]\n";
        return 0;
    }

    // Example: Using String utilities
    std::string text = "   Hello RSTools!   ";
    std::cout << RST::String::LtrimCopy(text) << "\n";

    log.LogPrint(RST::Log::LogLevel::Info, "Application started.");
    return 0;
}
