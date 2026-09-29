#pragma once

#include "RST/log/LogLevel.hpp"
#include "RST/log/SourceLocation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace RST::Log {

    /// A log record as passed to the sinks.
    ///
    /// Its strings are views, only valid during ISink::write(): copy the message into a
    /// LogMessageBuffer to keep it.
    struct LogMessage {
        std::chrono::system_clock::time_point time{};
        std::string_view category;
        std::string_view message;
        SourceLocation source{};
        std::uint64_t threadId = 0;
        LogLevel level = LogLevel::Info;

        LogMessage() = default;
        LogMessage(LogLevel level, std::string_view category, std::string_view message, SourceLocation source = {}) noexcept;
    };

    /// Copy of a LogMessage that owns its strings.
    class LogMessageBuffer {
    public:
        explicit LogMessageBuffer(const LogMessage& message);

        LogMessageBuffer(const LogMessageBuffer& other);
        LogMessageBuffer(LogMessageBuffer&& other) noexcept;
        LogMessageBuffer& operator=(const LogMessageBuffer& other);
        LogMessageBuffer& operator=(LogMessageBuffer&& other) noexcept;
        ~LogMessageBuffer() = default;

        [[nodiscard]] const LogMessage& get() const noexcept { return _message; }
        [[nodiscard]] const LogMessage* operator->() const noexcept { return &_message; }

    private:
        void rebind() noexcept;

        std::string _storage;
        std::size_t _messageOffset = 0;
        std::size_t _fileOffset = 0;
        std::size_t _funcOffset = 0;
        LogMessage _message;
    };

}
