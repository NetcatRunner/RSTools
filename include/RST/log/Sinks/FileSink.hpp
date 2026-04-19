#pragma once
#include "ASink.hpp"

#include <fstream>
#include <string>

namespace RST::Log {

class FileSink : public ASink {
private:
    std::string   _filepath;
    std::ofstream _file;
protected:
    void log(const LogMessage& msg) override;
public:
    FileSink(std::string_view filepath, bool truncate = false);
    ~FileSink() override;

    void flush() override;

    const std::string& filepath() const noexcept { return _filepath; };
};

} // namespace RST::Log
