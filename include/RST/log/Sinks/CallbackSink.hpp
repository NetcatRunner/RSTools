#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <functional>
#include <string_view>

namespace RST::Log {

    /// Calls a function for each message, with the LogMessage or with the level and formatted line.
    class CallbackSink : public ASink {
    public:
        using MessageCallback = std::function<void(const LogMessage&)>;
        using LineCallback = std::function<void(LogLevel, std::string_view)>;

        CallbackSink() = default;
        explicit CallbackSink(MessageCallback callback);
        explicit CallbackSink(LineCallback callback);

        void setCallback(MessageCallback callback);
        void setCallback(LineCallback callback);
        void clearCallback();

        [[nodiscard]] bool hasCallback();

    protected:
        void log(const LogMessage& message) override;

    private:
        MessageCallback _messageCallback;
        LineCallback _lineCallback;
    };

}
