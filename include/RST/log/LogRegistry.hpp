#pragma once

#include "RST/log/Log.hpp"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace RST::Log {

    class ISink;

    class Registry {
    public:
        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;

        static Registry& instance();

        [[nodiscard]] static std::shared_ptr<Logger> get(std::string_view name);
        [[nodiscard]] static std::shared_ptr<Logger> defaultLogger();

        [[nodiscard]] static Logger& defaultLoggerRef();

        static void setGlobalLevel(LogLevel level);
        static void addGlobalSink(std::shared_ptr<ISink> sink);
        static void reset();

        static std::shared_ptr<Logger> init(std::string_view appName = "app", LogLevel level = LogLevel::Info, bool toFile = false);

    private:
        struct NameHash {
            using is_transparent = void;
            [[nodiscard]] std::size_t operator()(std::string_view name) const noexcept { return std::hash<std::string_view>{}(name); }
        };

        Registry() = default;

        std::shared_ptr<Logger> getOrCreate(std::string_view name);

        std::mutex _mutex;
        std::unordered_map<std::string, std::shared_ptr<Logger>, NameHash, std::equal_to<>> _loggers;
        std::vector<std::shared_ptr<ISink>> _globalSinks;
        LogLevel _globalLevel = LogLevel::Trace;
        bool _initialized = false;
        std::atomic<Logger*> _default{nullptr};
    };

}

// ── Macros on the default logger ─────────────────────────────────────────────
#define RST_LOG(level, ...) RST_LOG_L(::RST::Log::Registry::defaultLoggerRef(), level, __VA_ARGS__)

#define RST_LOG_TRACE(...) RST_LOGGER_TRACE(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)
#define RST_LOG_DEBUG(...) RST_LOGGER_DEBUG(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)
#define RST_LOG_INFO(...)  RST_LOGGER_INFO(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)
#define RST_LOG_WARN(...)  RST_LOGGER_WARN(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)
#define RST_LOG_ERROR(...) RST_LOGGER_ERROR(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)
#define RST_LOG_FATAL(...) RST_LOGGER_FATAL(::RST::Log::Registry::defaultLoggerRef(), __VA_ARGS__)

#if defined(RST_LOG_LEGACY_MACROS)
#  define LOG_TRACE(...) RST_LOG_TRACE(__VA_ARGS__)
#  define LOG_DEBUG(...) RST_LOG_DEBUG(__VA_ARGS__)
#  define LOG_INFO(...)  RST_LOG_INFO(__VA_ARGS__)
#  define LOG_WARN(...)  RST_LOG_WARN(__VA_ARGS__)
#  define LOG_ERROR(...) RST_LOG_ERROR(__VA_ARGS__)
#  define LOG_FATAL(...) RST_LOG_FATAL(__VA_ARGS__)
#endif
