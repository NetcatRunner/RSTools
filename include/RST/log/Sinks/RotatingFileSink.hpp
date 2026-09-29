#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>

namespace RST::Log {

    /// Writes to a file and rotates it when it grows too large.
    class RotatingFileSink : public ASink {
    public:
        static constexpr std::size_t DEFAULT_MAX_SIZE  = 10 * 1024 * 1024;
        static constexpr std::size_t DEFAULT_MAX_FILES = 5;

        /// Starts a new file past `maxSize` bytes, keeping `maxFiles` backups; `<filepath>.1.log` is the newest.
        /// @throws std::runtime_error if the file cannot be opened.
        explicit RotatingFileSink(std::string_view filepath, std::size_t maxSize = DEFAULT_MAX_SIZE, std::size_t maxFiles = DEFAULT_MAX_FILES);
        ~RotatingFileSink() override;

        [[nodiscard]] std::size_t currentSize() const noexcept { return _currentSize; }
        [[nodiscard]] std::size_t maxSize() const noexcept { return _maxSize; }
        [[nodiscard]] std::size_t maxFiles() const noexcept { return _maxFiles; }

    protected:
        void log(const LogMessage& message) override;
        void flushSink() override;

    private:
        void rotate() noexcept;
        [[nodiscard]] std::string backupPath(std::size_t index) const;

        std::string _baseFilepath;
        std::ofstream _file;
        std::size_t _maxSize;
        std::size_t _maxFiles;
        std::size_t _currentSize = 0;
        bool _rotationFailureReported = false;
    };

}
