#include "RST/log/LogRegistry.hpp"
#include "RST/log/Sinks/ConsoleSink.hpp"
#include "RST/log/Sinks/FileSink.hpp"

namespace RST::Log {

Registry& Registry::instance() {
    static Registry inst;
    return inst;
}

std::shared_ptr<Logger> Registry::get(const std::string& name) {
    return instance().getOrCreate(name);
}

std::shared_ptr<Logger> Registry::defaultLogger() {
    return get("default");
}

void Registry::setGlobalLevel(LogLevel level) {
    auto& reg = instance();
    reg._globalLevel = level;
    for (auto& [name, logger] : reg._loggers)
        logger->setLevel(level);
}

void Registry::addGlobalSink(std::shared_ptr<ISink> sink) {
    auto& reg = instance();
    reg._globalSinks.push_back(sink);
    for (auto& [name, logger] : reg._loggers)
        logger->addSink(sink);
}

void Registry::reset() {
    auto& reg = instance();
    reg._loggers.clear();
    reg._globalSinks.clear();
    reg._globalLevel = LogLevel::Trace;
}

std::shared_ptr<Logger> Registry::init(const std::string& appName, LogLevel level, bool toFile) {
    auto& reg = instance();

    reg._globalLevel = level;

    auto console = std::make_shared<ConsoleSink>();
    console->setLevel(level);
    reg._globalSinks.push_back(console);

    if (toFile) {
        auto file = std::make_shared<FileSink>(appName + ".log");
        file->setLevel(level);
        reg._globalSinks.push_back(file);
    }

    auto it = reg._loggers.find("default");
    if (it == reg._loggers.end()) {
        auto logger = std::make_shared<Logger>("default", level);
        for (auto& s : reg._globalSinks)
            logger->addSink(s);
        reg._loggers["default"] = logger;
        return logger;
    }

    it->second->setLevel(level);
    for (auto& s : reg._globalSinks)
        it->second->addSink(s);
    return it->second;
}

std::shared_ptr<Logger> Registry::getOrCreate(const std::string& name) {
    auto it = _loggers.find(name);
    if (it != _loggers.end())
        return it->second;

    auto logger = std::make_shared<Logger>(name, _globalLevel);
    for (auto& sink : _globalSinks)
        logger->addSink(sink);

    _loggers[name] = logger;
    return logger;
}

} // namespace RST::Log
