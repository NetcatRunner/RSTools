#include "RST/log/Sinks/ASink.hpp"

#include "RST/log/Format/Formatter.hpp"

#include <utility>

namespace RST::Log {

    ASink::ASink() : _formatter(std::make_shared<Formatter>()) {}

    ASink::~ASink() = default;

    void ASink::write(const LogMessage& message)
    {
        if (!shouldLog(message.level)) {
            return;
        }
        std::lock_guard lock(_mutex);
        log(message);
    }

    void ASink::flush()
    {
        std::lock_guard lock(_mutex);
        flushSink();
    }

    void ASink::setFormatter(std::shared_ptr<Formatter> formatter)
    {
        if (formatter == nullptr) {
            return;
        }
        std::lock_guard lock(_mutex);
        _formatter = std::move(formatter);
    }

    void ASink::setPattern(std::string_view pattern)
    {
        setFormatter(std::make_shared<Formatter>(pattern));
    }

    std::string_view ASink::formatted(const LogMessage& message)
    {
        _line.clear();
        _formatter->format(message, _line);
        return _line;
    }

}
