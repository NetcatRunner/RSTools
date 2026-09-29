#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <cstddef>
#include <deque>
#include <vector>

namespace RST::Log {

    /// Keeps the latest messages in memory, to replay them into another sink later.
    class RingBufferSink : public ASink {
    public:
        static constexpr std::size_t DEFAULT_MAX_BYTES = 1024 * 1024;

        /// Keeps up to `maxMessages` messages and about `maxBytes` bytes, dropping the oldest first.
        explicit RingBufferSink(std::size_t maxMessages, std::size_t maxBytes = DEFAULT_MAX_BYTES);

        /// Writes the kept messages into `sink`, oldest first.
        void replayInto(ISink& sink);

        /// Copies of the kept messages, oldest first.
        [[nodiscard]] std::vector<LogMessageBuffer> snapshot();
        [[nodiscard]] std::size_t size();

    protected:
        void log(const LogMessage& message) override;

    private:
        std::deque<LogMessageBuffer> _messages;
        std::size_t _bytes = 0;
        std::size_t _maxMessages;
        std::size_t _maxBytes;
    };

}
