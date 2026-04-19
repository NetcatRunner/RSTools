#pragma once
#include "ASink.hpp"
#include <functional>

namespace RST::Log {

class CallbackSink : public ASink {
public:
    using Callback = std::function<void(const LogMessage&)>;

    CallbackSink(Callback cb): _callback(std::move(cb)){}

    void setCallback(Callback cb) {
        _callback = std::move(cb);
    }

protected:
    void log(const LogMessage& msg) override {
        if (_callback)
            _callback(msg);
    }

private:
    Callback _callback = nullptr;
};

} // namespace RST::Log
