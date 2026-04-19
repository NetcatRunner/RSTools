#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <memory>

#include "LogLevel.hpp"
#include "LogMessage.hpp"
#include "Sinks/ISink.hpp"

namespace RST::Log {

    inline void format_to(std::ostringstream& oss, std::string_view fmt) {
        oss << fmt;
    }

    template <typename T, typename... Rest>
    void format_to(std::ostringstream& oss, std::string_view fmt, T&& val, Rest&&... rest) {
        auto pos = fmt.find("{}");

        if (pos == std::string_view::npos) {
            oss << fmt;
            return;
        }

        oss << fmt.substr(0, pos);
        oss << std::forward<T>(val);
        format_to(oss, fmt.substr(pos + 2), std::forward<Rest>(rest)...);
    }

    template <typename... Args>
    std::string format(std::string_view fmt, Args&&... args) {
        std::ostringstream oss;
        format_to(oss, fmt, std::forward<Args>(args)...);
        return oss.str();
    }

    class Logger
    {
    private:
        std::string _name;
        LogLevel _level;
        std::vector<std::shared_ptr<ISink>> _sinks;

        template <typename... Args>
        void logFmt(LogLevel lvl, SourceLocation src, std::string_view fmt, Args&&... args) {
            if (!shouldLog(lvl))
                return;
            dispatch(LogMessage(lvl, _name, format(fmt, std::forward<Args>(args)...), src));
        }

        void dispatch(const LogMessage& msg);
    public:
        Logger(std::string name, LogLevel level = LogLevel::Trace): _name(std::move(name)), _level(level) {};

        void addSink(std::shared_ptr<ISink> sink) {_sinks.push_back(std::move(sink));};
        void clearSinks() {_sinks.clear();};
        size_t sinkCount() const {return _sinks.size();};

        void setLevel(LogLevel level) {_level = level;};
        LogLevel getLevel() const {return _level;};
        bool shouldLog(LogLevel lvl) const {return lvl >= _level;};

        const std::string& getName() const { return _name; };

        template <typename... Args>
        void trace(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Trace, {}, fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void debug(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Debug, {}, fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void info(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Info, {}, fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void warn(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Warn, {}, fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void error(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Error, {}, fmt, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void fatal(std::string_view fmt, Args&&... args) {
            logFmt(LogLevel::Fatal, {}, fmt, std::forward<Args>(args)...);
        }


        template <typename... Args>
        void log(LogLevel lvl, SourceLocation src, std::string_view fmt, Args&&... args) {
            logFmt(lvl, src, fmt, std::forward<Args>(args)...);
        }

        void log(LogLevel lvl, std::string_view message, SourceLocation src = {});

        void flush();
    };

}
