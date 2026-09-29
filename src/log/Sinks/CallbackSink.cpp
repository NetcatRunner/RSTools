#include "RST/log/Sinks/CallbackSink.hpp"

#include <mutex>
#include <utility>

namespace RST::Log {

    CallbackSink::CallbackSink(MessageCallback callback) : _messageCallback(std::move(callback)) {}

    CallbackSink::CallbackSink(LineCallback callback) : _lineCallback(std::move(callback)) {}

    void CallbackSink::setCallback(MessageCallback callback)
    {
        std::lock_guard lock(sinkMutex());
        _messageCallback = std::move(callback);
        _lineCallback = nullptr;
    }

    void CallbackSink::setCallback(LineCallback callback)
    {
        std::lock_guard lock(sinkMutex());
        _lineCallback = std::move(callback);
        _messageCallback = nullptr;
    }

    void CallbackSink::clearCallback()
    {
        std::lock_guard lock(sinkMutex());
        _messageCallback = nullptr;
        _lineCallback = nullptr;
    }

    bool CallbackSink::hasCallback()
    {
        std::lock_guard lock(sinkMutex());
        return static_cast<bool>(_messageCallback) || static_cast<bool>(_lineCallback);
    }

    void CallbackSink::log(const LogMessage& message)
    {
        if (_messageCallback) {
            _messageCallback(message);
        }
        if (_lineCallback) {
            _lineCallback(message.level, formatted(message));
        }
    }

}
