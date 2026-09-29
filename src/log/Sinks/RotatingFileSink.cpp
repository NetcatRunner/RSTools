#include "RST/log/Sinks/RotatingFileSink.hpp"

#include "log/detail/Platform.hpp"

#include <cstdio>
#include <ios>
#include <stdexcept>

namespace RST::Log {

    RotatingFileSink::RotatingFileSink(std::string_view filepath, std::size_t maxSize, std::size_t maxFiles)
        : _baseFilepath(filepath), _maxSize(maxSize == 0 ? DEFAULT_MAX_SIZE : maxSize), _maxFiles(maxFiles)
    {
        _file.open(_baseFilepath, std::ios::out | std::ios::binary | std::ios::app);
        if (!_file.is_open()) {
            throw std::runtime_error("RotatingFileSink: cannot open " + _baseFilepath);
        }
        _file.seekp(0, std::ios::end);
        const std::streamoff size = _file.tellp();
        _currentSize = size > 0 ? static_cast<std::size_t>(size) : 0;
    }

    RotatingFileSink::~RotatingFileSink()
    {
        if (_file.is_open()) {
            _file.flush();
        }
    }

    std::string RotatingFileSink::backupPath(std::size_t index) const
    {
        return _baseFilepath + '.' + std::to_string(index) + ".log";
    }

    void RotatingFileSink::rotate() noexcept
    {
        try {
            _file.flush();
            _file.close();

            if (_maxFiles > 0) {
                std::remove(backupPath(_maxFiles).c_str());
                for (std::size_t index = _maxFiles - 1; index >= 1; --index) {
                    std::rename(backupPath(index).c_str(), backupPath(index + 1).c_str());
                }
                std::rename(_baseFilepath.c_str(), backupPath(1).c_str());
            }

            _file.open(_baseFilepath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (_file.is_open()) {
                _currentSize = 0;
                return;
            }
            _file.open(_baseFilepath, std::ios::out | std::ios::binary | std::ios::app);
        } catch (...) {
        }

        if (!_rotationFailureReported) {
            _rotationFailureReported = true;
            detail::reportInternalError("RotatingFileSink: rotation failed, writing on without rotating", _baseFilepath);
        }
    }

    void RotatingFileSink::log(const LogMessage& message)
    {
        const std::string_view line = formatted(message);
        const std::size_t bytes = line.size() + 1;

        if (_currentSize > 0 && _currentSize + bytes > _maxSize) {
            rotate();
        }

        _file.write(line.data(), static_cast<std::streamsize>(line.size()));
        _file.put('\n');
        _currentSize += bytes;
    }

    void RotatingFileSink::flushSink()
    {
        _file.flush();
    }

}
