#include <RST/RST.hpp>

#include <memory>
#include <vector>

using namespace RST::Log;

void demo_sinks() {
    auto logger = std::make_shared<Logger>("Sinks", LogLevel::Trace);

    auto console = std::make_shared<ConsoleSink>(std::cerr);
    console->setLevel(LogLevel::Warn);
    console->setPattern("[%H:%M:%S] [%L] %v");
    logger->addSink(console);

    auto rotFile = std::make_shared<RotatingFileSink>("app.log", 5'000'000, 3);
    rotFile->setLevel(LogLevel::Debug);
    rotFile->setPattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [%f:%#] %v");
    logger->addSink(rotFile);

    logger->debug("Ceci va dans le fichier seulement");
    logger->warn("Ceci va partout — fichier ET console");
    logger->error("Erreur {}", "critique");
}

void demo_patterns() {
    auto logger = std::make_shared<Logger>("Patterns", LogLevel::Trace);
    auto console = std::make_shared<ConsoleSink>();
    logger->addSink(console);

    console->setPattern("[%L] %v");
    logger->info("Pattern minimaliste");

    console->setPattern("[%H:%M:%S.%e] [T:%t] [%L] %v");
    logger->debug("Avec thread ID");

    console->setPattern("[%l] %v  (%f:%#)");
    LOG_INFO_L(logger, "Avec source location");
}

void demo_registry() {
    Registry::init("MyService", LogLevel::Info, true);

    auto db  = Registry::get("Database");
    auto api = Registry::get("API");
    auto rdr = Registry::get("Renderer");

    db->info("Pool de connexions initialisé — size={}", 10);
    api->info("Serveur HTTP écoutant sur :{}", 8080);
    rdr->warn("Mode software rendering (GPU non disponible)");

    Registry::setGlobalLevel(LogLevel::Debug);
    db->debug("Requête SQL : SELECT * FROM users LIMIT 10");
}

int main() {
    std::cout << "\n=== Demo Sinks ===\n";
    demo_sinks();

    std::cout << "\n=== Demo Patterns ===\n";
    demo_patterns();

    std::cout << "\n=== Demo Registry ===\n";
    demo_registry();

    return 0;
}
