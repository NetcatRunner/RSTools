#pragma once
#include "ASink.hpp"

#include <functional>
#include <string_view>
#include <utility>

namespace RST::Log {

    class CallbackSink : public ASink {
    public:
        using MessageCallback = std::function<void(const LogMessage&)>;
        using LineCallback = std::function<void(LogLevel, std::string_view)>;

        CallbackSink() = default;
        explicit CallbackSink(MessageCallback cb) : _messageCallback(std::move(cb)) {}
        explicit CallbackSink(LineCallback cb) : _lineCallback(std::move(cb)) {}

        void setCallback(MessageCallback cb) {
            _messageCallback = std::move(cb);
            _lineCallback = nullptr;
        }

        void setCallback(LineCallback cb) {
            _lineCallback = std::move(cb);
            _messageCallback = nullptr;
        }

        void clearCallback() noexcept {
            _messageCallback = nullptr;
            _lineCallback = nullptr;
        }

        [[nodiscard]] bool hasCallback() const noexcept { return static_cast<bool>(_messageCallback) || static_cast<bool>(_lineCallback); }

    protected:
        void log(const LogMessage& msg) override {
            if (_messageCallback) {
                _messageCallback(msg);
            }
            if (_lineCallback) {
                _lineCallback(msg.level, formatted(msg));
            }
        }

    private:
        MessageCallback _messageCallback = nullptr;
        LineCallback _lineCallback = nullptr;
    };

}
