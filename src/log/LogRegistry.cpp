#include "RST/log/LogRegistry.hpp"

#include "RST/log/Sinks/ConsoleSink.hpp"
#include "RST/log/Sinks/FileSink.hpp"

#include <string>
#include <utility>

namespace RST::Log {

    Registry& Registry::instance()
    {
        static Registry registry;
        return registry;
    }

    std::shared_ptr<Logger> Registry::get(std::string_view name)
    {
        Registry& registry = instance();
        std::lock_guard lock(registry._mutex);
        return registry.getOrCreate(name);
    }

    std::shared_ptr<Logger> Registry::defaultLogger()
    {
        return get("default");
    }

    Logger& Registry::defaultLoggerRef()
    {
        Registry& registry = instance();
        if (Logger* logger = registry._default.load(std::memory_order_acquire); logger != nullptr) {
            return *logger;
        }
        std::lock_guard lock(registry._mutex);
        return *registry.getOrCreate("default");
    }

    void Registry::setGlobalLevel(LogLevel level)
    {
        Registry& registry = instance();
        std::lock_guard lock(registry._mutex);
        registry._globalLevel = level;
        for (auto& [name, logger] : registry._loggers) {
            logger->setLevel(level);
        }
    }

    void Registry::addGlobalSink(std::shared_ptr<ISink> sink)
    {
        if (sink == nullptr) {
            return;
        }
        Registry& registry = instance();
        std::lock_guard lock(registry._mutex);
        registry._globalSinks.push_back(sink);
        for (auto& [name, logger] : registry._loggers) {
            logger->addSink(sink);
        }
    }

    void Registry::reset()
    {
        Registry& registry = instance();
        std::lock_guard lock(registry._mutex);
        registry._default.store(nullptr, std::memory_order_release);
        registry._loggers.clear();
        registry._globalSinks.clear();
        registry._globalLevel = LogLevel::Trace;
        registry._initialized = false;
    }

    std::shared_ptr<Logger> Registry::init(std::string_view appName, LogLevel level, bool toFile)
    {
        Registry& registry = instance();
        std::lock_guard lock(registry._mutex);

        if (!registry._initialized) {
            std::vector<std::shared_ptr<ISink>> sinks{std::make_shared<ConsoleSink>()};
            if (toFile) {
                sinks.push_back(std::make_shared<FileSink>(std::string(appName) + ".log"));
            }
            for (const std::shared_ptr<ISink>& sink : sinks) {
                registry._globalSinks.push_back(sink);
                for (auto& [name, logger] : registry._loggers) {
                    logger->addSink(sink);
                }
            }
            registry._initialized = true;
        }

        registry._globalLevel = level;
        for (auto& [name, logger] : registry._loggers) {
            logger->setLevel(level);
        }
        return registry.getOrCreate("default");
    }

    std::shared_ptr<Logger> Registry::getOrCreate(std::string_view name)
    {
        if (const auto found = _loggers.find(name); found != _loggers.end()) {
            return found->second;
        }

        auto logger = std::make_shared<Logger>(std::string(name), _globalLevel);
        for (const std::shared_ptr<ISink>& sink : _globalSinks) {
            logger->addSink(sink);
        }
        _loggers.emplace(std::string(name), logger);
        if (name == "default") {
            _default.store(logger.get(), std::memory_order_release);
        }
        return logger;
    }

}
