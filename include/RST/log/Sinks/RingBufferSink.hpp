#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <cstddef>
#include <deque>
#include <vector>

namespace RST::Log {

    class RingBufferSink : public ASink {
    public:
        static constexpr std::size_t DEFAULT_MAX_BYTES = 1024 * 1024;

        explicit RingBufferSink(std::size_t maxMessages, std::size_t maxBytes = DEFAULT_MAX_BYTES);

        void replayInto(ISink& sink);

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
