#pragma once

#include "../LogMessage.hpp"
#include <string>

namespace RST::Log {
    class Formatter
    {
    private:
        /* data */
    public:
        Formatter(const std::string& pattern = "[%l] [%c] %v");
        ~Formatter();

        void format(const LogMessage &msg, std::string &dest);
    };

}

