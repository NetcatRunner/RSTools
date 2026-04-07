#include "RST/log/Format/Formatter.hpp"

namespace RST::Log {

    Log::Formatter::Formatter([[maybe_unused]] const std::string& pattern)
    {

    }
        
    Log::Formatter::~Formatter()
    {
    }

    void Log::Formatter::format([[maybe_unused]]const LogMessage &msg,[[maybe_unused]] std::string &dest)
    {
        
    }
}