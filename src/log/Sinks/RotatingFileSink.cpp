#include "RST/log/Sinks/RotatingFileSink.hpp"
#include <stdexcept>
#include <cstdio>
#include <string>

namespace RST::Log {

RotatingFileSink::RotatingFileSink(std::string_view filepath, size_t max_size, size_t max_files): _baseFilepath(filepath), _maxSize(max_size), _maxFiles(max_files) {
    openFile();
}

RotatingFileSink::~RotatingFileSink() {
    if (_file.is_open()) {
        _file.flush();
        _file.close();
    }
}

void RotatingFileSink::openFile() {
    _file.open(_baseFilepath, std::ios::out | std::ios::app);
    if (!_file.is_open())
        throw std::runtime_error("RotatingFileSink: fail to open " + _baseFilepath);

    _file.seekp(0, std::ios::end);
    _currentSize = static_cast<size_t>(_file.tellp());
}

void RotatingFileSink::rotate() {
    _file.flush();
    _file.close();

    std::string oldest = _baseFilepath + '.' + std::to_string(_maxFiles) + ".log";
    std::remove(oldest.c_str());

    for (size_t i = _maxFiles - 1; i >= 1; --i) {
        std::string src  = _baseFilepath + '.' + std::to_string(i)     + ".log";
        std::string dst  = _baseFilepath + '.' + std::to_string(i + 1) + ".log";
        std::rename(src.c_str(), dst.c_str());
    }

    std::string first = _baseFilepath + ".1.log";
    std::rename(_baseFilepath.c_str(), first.c_str());

    _file.open(_baseFilepath, std::ios::out | std::ios::trunc);
    if (!_file.is_open())
        throw std::runtime_error("RotatingFileSink: rotation failed " + _baseFilepath);

    _currentSize = 0;
}

void RotatingFileSink::log(const LogMessage& msg) {
    std::string line = formatted(msg);
    line += '\n';

    if (_currentSize + line.size() > _maxSize)
        rotate();

    _file << line;
    _currentSize += line.size();
}

void RotatingFileSink::flush() {
    _file.flush();
}

} // namespace RST::Log
