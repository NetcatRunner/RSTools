#pragma once

#include "ASink.hpp"

#include <fstream>

namespace RST::Log {

    class FileSink : public ASink
    {
    private:
        std::ofstream m_File;

    public:

        FileSink(const std::string& path): m_File(path, std::ios::app) {}

        void log(const LogMessage& msg) override;
    };

}
