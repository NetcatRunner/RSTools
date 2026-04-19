#pragma once

#include "Logger.hpp"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace RST::Log {

    class Registry {
    public:
        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;

        static Registry& instance();

        static std::shared_ptr<Logger> get(const std::string& name);

        static std::shared_ptr<Logger> defaultLogger();

        static void setGlobalLevel(LogLevel level);

        static void addGlobalSink(std::shared_ptr<ISink> sink);

        static void reset();

        static std::shared_ptr<Logger> init(const std::string& appName = "app", LogLevel level = LogLevel::Info, bool toFile = false);

    private:
        Registry() = default;

        std::shared_ptr<Logger> getOrCreate(const std::string& name);

        std::unordered_map<std::string, std::shared_ptr<Logger>> _loggers;
        std::vector<std::shared_ptr<ISink>> _globalSinks;
        LogLevel _globalLevel = LogLevel::Trace;
    };

} // namespace RST::Log
