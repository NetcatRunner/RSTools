#pragma once
#include "ASink.hpp"
#include <fstream>
#include <string>
#include <cstddef>

namespace RST::Log {

class RotatingFileSink : public ASink {
public:
    static constexpr size_t DEFAULT_MAX_SIZE  = 10 * 1024 * 1024;
    static constexpr size_t DEFAULT_MAX_FILES = 5;

    RotatingFileSink(std::string_view  filepath, size_t max_size  = DEFAULT_MAX_SIZE, size_t max_files = DEFAULT_MAX_FILES);
    ~RotatingFileSink() override;

    void flush() override;

    size_t currentSize() const { return _currentSize; };
    size_t maxSize() const { return _maxSize; };
    size_t maxFiles() const { return _maxFiles; };

protected:
    void log(const LogMessage& msg) override;

private:
    void rotate();
    void openFile();

    std::string   _baseFilepath;
    std::ofstream _file;
    size_t        _maxSize;
    size_t        _maxFiles;
    size_t        _currentSize = 0;
};

} // namespace RST::Log
