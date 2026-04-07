#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <memory>

#include "Sinks/Sink.hpp"
#include "LogMessage.hpp"
#include "LogLevel.hpp"

namespace RST::Log {

    class Log
    {
    private:
        const std::string _name;
        std::vector<sink_ptr> _sinks;
    public:
        Log() = delete;
        Log(std::string name) : _name(name) {};
        Log(std::string name, std::vector<sink_ptr>::iterator begin, std::vector<sink_ptr>::iterator end): _name(name), _sinks(begin, end) {};
        Log(std::string name, std::vector<sink_ptr> sinks): _name(name), _sinks(sinks.begin(), sinks.end()) {};
        Log(std::string name, sink_ptr sink): _name(std::move(name)), _sinks{std::move(sink)} {};

        void AddSink(sink_ptr sink) { _sinks.push_back(sink); };

        template<typename... Args>
        void LogPrint(LogLevel level,  Args&&... args)
        {
            if (_sinks.empty())
                return;

            std::stringstream ss;
            (ss << ... << args);

            LogMessage msg = {level, ss.str(),_name, std::chrono::system_clock::now()};

            for (const auto& sink : _sinks) {
                sink->log(msg);
            }
        }
    };
    using log_ptr = std::shared_ptr<Log>;

}