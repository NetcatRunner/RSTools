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

    try {
        parser.parse(argc, argv);
    } catch (const RST::Parser::HelpRequested& h) {
        parser.printHelp();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[error] " << e.what() << "\n\n";
        parser.printHelp();
        return 1;
    }

    // Example: Using String utilities
    std::string text = "   Hello RSTools!   ";
    std::cout << RST::String::LtrimCopy(text) << "\n";

    log.LogPrint(RST::Log::LogLevel::Info, "Application started.");
    return 0;
}
