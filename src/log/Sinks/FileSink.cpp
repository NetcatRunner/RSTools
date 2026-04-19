#include "RST/log/Sinks/FileSink.hpp"
#include <stdexcept>

namespace RST::Log {

FileSink::FileSink(std::string_view filepath, bool truncate): _filepath(filepath)
{
    auto mode = std::ios::out | (truncate ? std::ios::trunc : std::ios::app);
    _file.open(_filepath, mode);
    if (!_file.is_open())
        throw std::runtime_error("FileSink: impossible to open " + _filepath);
}

FileSink::~FileSink() {
    if (_file.is_open()) {
        _file.flush();
        _file.close();
    }
}

void FileSink::log(const LogMessage& msg) {
    _file << formatted(msg) << '\n';
}

void FileSink::flush() {
    _file.flush();
}

} // namespace RST::Log
