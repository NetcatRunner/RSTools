#include "RST/log/LogLevel.hpp"

#include <ostream>

namespace RST::Log {

    std::ostream& operator<<(std::ostream& out, LogLevel level)
    {
        return out << toString(level);
    }

}
