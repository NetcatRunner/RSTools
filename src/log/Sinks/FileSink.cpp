#include "RST/log/Sinks/FileSink.hpp"

#include <ios>
#include <stdexcept>

namespace RST::Log {

    FileSink::FileSink(std::string_view filepath, bool truncate)
        : _filepath(filepath)
    {
        const std::ios::openmode mode = std::ios::out | std::ios::binary | (truncate ? std::ios::trunc : std::ios::app);
        _file.open(_filepath, mode);
        if (!_file.is_open()) {
            throw std::runtime_error("FileSink: cannot open " + _filepath);
        }
    }

    FileSink::~FileSink()
    {
        if (_file.is_open()) {
            _file.flush();
        }
    }

    void FileSink::log(const LogMessage& message)
    {
        const std::string_view line = formatted(message);
        _file.write(line.data(), static_cast<std::streamsize>(line.size()));
        _file.put('\n');
    }

    void FileSink::flushSink()
    {
        _file.flush();
    }

}
