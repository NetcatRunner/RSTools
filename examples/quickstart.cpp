#include <RST/RST.hpp>

#include <memory>
#include <string>

int main(int argc, char** argv) {
    // Arguments: -h/--help and errors are handled by the parser, nothing throws
    RST::Parser::ArgParser parser("quickstart", "A short tour of RSTools.");
    parser.addFlag({"-v", "--verbose"}, "Print more details");
    parser.addOption({"-n", "--count"}, "How many greetings").integer().range(1, 10).defaultValue(1);

    if (auto result = parser.parse(argc, argv); !result) {
        result.print();
        return result.exitCode();
    }

    // Logging
    RST::Log::Logger logger("App");
    logger.addSink(std::make_shared<RST::Log::ConsoleSink>());
    logger.setLevel(parser.isSet("--verbose") ? RST::Log::LogLevel::Debug : RST::Log::LogLevel::Info);

    // Strings
    const std::string greeting = RST::String::trimCopy("   Hello RSTools!   ");
    for (int i = 0; i < parser.get<int>("--count").value_or(1); ++i)
        logger.info("{}", greeting);

    logger.debug("{} with {} cores", RST::System::getOSName(), RST::System::getCpuCores());
    return 0;
}
