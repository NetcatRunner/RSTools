#include "RST/log/Logger.hpp"

#include "RST/log/LogMessage.hpp"
#include "RST/log/Sinks/ISink.hpp"
#include "RST/log/Sinks/RingBufferSink.hpp"

#include "log/detail/MemoryBuffer.hpp"
#include "log/detail/Platform.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace RST::Log {

    struct Logger::SinkList {
        mutable std::shared_mutex mutex;
        std::vector<std::shared_ptr<ISink>> sinks;
    };

    namespace {

        void writeFormatError(detail::MemoryBuffer& buffer, std::string_view format, std::string_view reason) noexcept
        {
            buffer.clear();
            try {
                buffer.append("[RST::Log] cannot format \"");
                buffer.append(format);
                buffer.append("\": ");
                buffer.append(reason);
            } catch (...) {
                buffer.clear();
            }
        }

        void formatInto(detail::MemoryBuffer& buffer, std::string_view format, std::format_args args) noexcept
        {
            try {
                std::vformat_to(std::back_inserter(buffer), format, args);
            } catch (const std::exception& error) {
                writeFormatError(buffer, format, error.what());
            } catch (...) {
                writeFormatError(buffer, format, "unknown exception");
            }
        }

    }

    Logger::Logger(std::string name, LogLevel level)
        : _name(std::move(name)), _level(level), _sinks(std::make_unique<SinkList>())
    {}

    Logger::~Logger() = default;

    void Logger::addSink(std::shared_ptr<ISink> sink)
    {
        if (sink == nullptr) {
            return;
        }
        std::unique_lock lock(_sinks->mutex);
        _sinks->sinks.push_back(std::move(sink));
    }

    void Logger::addSink(std::shared_ptr<ISink> sink, RingBufferSink& history)
    {
        if (sink == nullptr) {
            return;
        }
        std::unique_lock lock(_sinks->mutex);
        try {
            history.replayInto(*sink);
        } catch (const std::exception& error) {
            detail::reportInternalError(_name, error.what());
        } catch (...) {
            detail::reportInternalError(_name, "a sink threw while replaying the history");
        }
        _sinks->sinks.push_back(std::move(sink));
    }

    bool Logger::removeSink(const std::shared_ptr<ISink>& sink) noexcept
    {
        std::shared_ptr<ISink> released;
        {
            std::unique_lock lock(_sinks->mutex);
            const auto found = std::ranges::find(_sinks->sinks, sink);
            if (found == _sinks->sinks.end()) {
                return false;
            }
            released = std::move(*found);
            _sinks->sinks.erase(found);
        }
        return true;
    }

    void Logger::clearSinks() noexcept
    {
        std::vector<std::shared_ptr<ISink>> released;
        {
            std::unique_lock lock(_sinks->mutex);
            released.swap(_sinks->sinks);
        }
    }

    std::size_t Logger::sinkCount() const noexcept
    {
        std::shared_lock lock(_sinks->mutex);
        return _sinks->sinks.size();
    }

    void Logger::vlog(LogLevel level, const SourceLocation& source, std::string_view category, std::string_view format, std::format_args args) noexcept
    {
        if (!shouldLog(level)) {
            return;
        }
        detail::MemoryBuffer buffer;
        formatInto(buffer, format, args);
        dispatch(LogMessage(level, category, buffer.view(), source));
    }

    void Logger::log(LogLevel level, std::string_view message, const SourceLocation& source) noexcept
    {
        if (!shouldLog(level)) {
            return;
        }
        dispatch(LogMessage(level, _name, message, source));
    }

    void Logger::flush() noexcept
    {
        std::shared_lock lock(_sinks->mutex);
        for (const std::shared_ptr<ISink>& sink : _sinks->sinks) {
            try {
                sink->flush();
            } catch (const std::exception& error) {
                detail::reportInternalError(_name, error.what());
            } catch (...) {
                detail::reportInternalError(_name, "a sink threw while flushing");
            }
        }
    }

    void Logger::dispatch(const LogMessage& message) noexcept
    {
        const LogLevel flushLevel = getFlushLevel();
        const bool flushAfter = flushLevel != LogLevel::Off && message.level >= flushLevel;

        std::shared_lock lock(_sinks->mutex);
        for (const auto& sink : _sinks->sinks) {
            try {
                if (sink->shouldLog(message.level)) {
                    sink->write(message);
                }
                if (flushAfter) {
                    sink->flush();
                }
            } catch (const std::exception& error) {
                detail::reportInternalError(_name, error.what());
            } catch (...) {
                detail::reportInternalError(_name, "a sink threw while writing");
            }
        }
    }

}
