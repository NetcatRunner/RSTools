#include "RST/log/Sinks/RingBufferSink.hpp"

#include <mutex>

namespace RST::Log {

    namespace {

        [[nodiscard]] std::size_t weightOf(const LogMessage& message) noexcept
        {
            std::size_t bytes = message.category.size() + message.message.size();
            if (message.source.valid()) {
                bytes += std::string_view(message.source.file).size();
                bytes += message.source.func != nullptr ? std::string_view(message.source.func).size() : 0;
            }
            return bytes + sizeof(LogMessage);
        }

    }

    RingBufferSink::RingBufferSink(std::size_t maxMessages, std::size_t maxBytes)
        : _maxMessages(maxMessages == 0 ? 1 : maxMessages), _maxBytes(maxBytes)
    {}

    void RingBufferSink::replayInto(ISink& sink)
    {
        std::lock_guard lock(sinkMutex());
        for (const LogMessageBuffer& kept : _messages) {
            if (sink.shouldLog(kept->level)) {
                sink.write(kept.get());
            }
        }
    }

    std::vector<LogMessageBuffer> RingBufferSink::snapshot()
    {
        std::lock_guard lock(sinkMutex());
        return {_messages.begin(), _messages.end()};
    }

    std::size_t RingBufferSink::size()
    {
        std::lock_guard lock(sinkMutex());
        return _messages.size();
    }

    void RingBufferSink::log(const LogMessage& message)
    {
        _messages.emplace_back(message);
        _bytes += weightOf(message);

        while (_messages.size() > _maxMessages || (_bytes > _maxBytes && _messages.size() > 1)) {
            _bytes -= weightOf(_messages.front().get());
            _messages.pop_front();
        }
    }

}
