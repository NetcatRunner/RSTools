#include "RST/log/LogMessage.hpp"

#include "log/detail/Platform.hpp"

#include <utility>

namespace RST::Log {

    LogMessage::LogMessage(LogLevel messageLevel, std::string_view messageCategory, std::string_view text, SourceLocation location) noexcept
        : time(std::chrono::system_clock::now()),
          category(messageCategory),
          message(text),
          source(location),
          threadId(detail::currentThreadId()),
          level(messageLevel)
    {}

    LogMessageBuffer::LogMessageBuffer(const LogMessage& message)
        : _message(message)
    {
        const std::string_view file = message.source.valid() ? std::string_view(message.source.file) : std::string_view{};
        const std::string_view func = message.source.func != nullptr ? std::string_view(message.source.func) : std::string_view{};

        _storage.reserve(message.category.size() + message.message.size() + file.size() + func.size() + 4);
        _storage.append(message.category).push_back('\0');
        _messageOffset = _storage.size();
        _storage.append(message.message).push_back('\0');
        _fileOffset = _storage.size();
        _storage.append(file).push_back('\0');
        _funcOffset = _storage.size();
        _storage.append(func).push_back('\0');
        rebind();
    }

    LogMessageBuffer::LogMessageBuffer(const LogMessageBuffer& other)
        : _storage(other._storage),
          _messageOffset(other._messageOffset),
          _fileOffset(other._fileOffset),
          _funcOffset(other._funcOffset),
          _message(other._message)
    {
        rebind();
    }

    LogMessageBuffer::LogMessageBuffer(LogMessageBuffer&& other) noexcept
        : _storage(std::move(other._storage)),
          _messageOffset(other._messageOffset),
          _fileOffset(other._fileOffset),
          _funcOffset(other._funcOffset),
          _message(other._message)
    {
        rebind();
    }

    LogMessageBuffer& LogMessageBuffer::operator=(const LogMessageBuffer& other)
    {
        if (this != &other) {
            _storage = other._storage;
            _messageOffset = other._messageOffset;
            _fileOffset = other._fileOffset;
            _funcOffset = other._funcOffset;
            _message = other._message;
            rebind();
        }
        return *this;
    }

    LogMessageBuffer& LogMessageBuffer::operator=(LogMessageBuffer&& other) noexcept
    {
        if (this != &other) {
            _storage = std::move(other._storage);
            _messageOffset = other._messageOffset;
            _fileOffset = other._fileOffset;
            _funcOffset = other._funcOffset;
            _message = other._message;
            rebind();
        }
        return *this;
    }

    void LogMessageBuffer::rebind() noexcept
    {
        const char* base = _storage.data();
        _message.category = std::string_view(base, _messageOffset - 1);
        _message.message = std::string_view(base + _messageOffset, _fileOffset - _messageOffset - 1);
        if (_message.source.valid()) {
            _message.source.file = base + _fileOffset;
            _message.source.func = base + _funcOffset;
        }
    }

}
