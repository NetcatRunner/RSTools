#include <RST/RST.hpp>

#include <memory>
#include <vector>

using namespace RST::Log;

void demoSinks() {
    auto logger = std::make_shared<Logger>("Sinks", LogLevel::Trace);

    auto console = std::make_shared<ConsoleSink>(std::cerr);
    console->setLevel(LogLevel::Warn);
    console->setPattern("[%H:%M:%S] [%L] %v");
    logger->addSink(console);

    auto rotFile = std::make_shared<RotatingFileSink>("app.log", 5'000'000, 3);
    rotFile->setLevel(LogLevel::Debug);
    rotFile->setPattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [%f:%#] %v");
    logger->addSink(rotFile);

    logger->debug("Only in the file");
    logger->warn("Everywhere: file and console");
    logger->error("Error {}", "critical");
}

void demoPatterns() {
    auto logger = std::make_shared<Logger>("Patterns", LogLevel::Trace);
    auto console = std::make_shared<ConsoleSink>();
    logger->addSink(console);

    console->setPattern("[%L] %v");
    logger->info("Minimal pattern");

    console->setPattern("[%H:%M:%S.%e] [T:%t] [%L] %v");
    logger->debug("With the thread id");

    console->setPattern("[%l] %v  (%f:%#)");
    RST_LOGGER_INFO(logger, "With the source location");
}

void demoRegistry() {
    Registry::init("MyService", LogLevel::Info, true);

    auto db  = Registry::get("Database");
    auto api = Registry::get("API");
    auto rdr = Registry::get("Renderer");

    db->info("Connection pool ready, size={}", 10);
    api->info("HTTP server listening on :{}", 8080);
    rdr->warn("Software rendering (no GPU available)");

    Registry::setGlobalLevel(LogLevel::Debug);
    db->debug("SQL query: SELECT * FROM users LIMIT 10");
}

int main() {
    std::cout << "\n=== Demo Sinks ===\n";
    demoSinks();

    std::cout << "\n=== Demo Patterns ===\n";
    demoPatterns();

    std::cout << "\n=== Demo Registry ===\n";
    demoRegistry();

    return 0;
}
