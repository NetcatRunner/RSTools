#pragma once

#include "RST/log/Sinks/ASink.hpp"

#include <fstream>
#include <string>
#include <string_view>

namespace RST::Log {

    /// Appends messages to a file.
    class FileSink : public ASink {
    public:
        /// Opens `filepath`, emptying it first if `truncate` is true.
        /// @throws std::runtime_error if the file cannot be opened.
        explicit FileSink(std::string_view filepath, bool truncate = false);
        ~FileSink() override;

        [[nodiscard]] const std::string& filepath() const noexcept { return _filepath; }

    protected:
        void log(const LogMessage& message) override;
        void flushSink() override;

    private:
        std::string _filepath;
        std::ofstream _file;
    };

}
