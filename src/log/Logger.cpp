#include "RST/log/Logger.hpp"


namespace RST::Log {

    void Logger::log(LogLevel lvl, std::string_view message, SourceLocation src) {
        if (!shouldLog(lvl))
            return;
        dispatch(LogMessage(lvl, _name, std::string(message), src));
    }

    void Logger::dispatch(const LogMessage& msg) {
        for (auto& sink : _sinks)
            sink->write(msg);
    }

    void Logger::flush() {
        for (auto& sink : _sinks)
            sink->flush();
    }

}
