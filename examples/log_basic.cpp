#include <RST/RST.hpp>
#include <memory>

int main() {
    // ── 1. Init ────────────────────
    auto log = RST::Log::Registry::init("MyApp", RST::Log::LogLevel::Debug);

    // ── 2. Logs ───────────────────────────────────────
    log->trace("Not shown: the level is Debug");
    log->debug("Starting the engine, threads={}", 4);
    log->info("Application started, v{}.{}", 1, 0);
    log->warn("Low memory: {}MB left", 128);
    log->error("File not found: {}", "config.json");

    // ── 3. Macros  ─────────────
    RST_LOG_INFO("RST_LOG_INFO records the file and line");
    RST_LOG_WARN("Unexpected value {}", 42);
    RST_LOG_ERROR("Critical error in the '{}' module", "Renderer");

    // ── 4. Loggers  ──────────────────────────────────────────
    auto net = RST::Log::Registry::get("Network");
    net->info("Connecting to {}:{}", "192.168.1.1", 8080);
    net->warn("Timeout after {}ms", 5000);

    return 0;
}
