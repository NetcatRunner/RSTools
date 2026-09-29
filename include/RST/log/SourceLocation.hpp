#pragma once

namespace RST::Log {

    /// File, function and line of a log call.
    struct SourceLocation {
        const char* file = nullptr;
        const char* func = nullptr;
        int line = 0;

        [[nodiscard]] constexpr bool valid() const noexcept { return file != nullptr; }
    };

}

/// SourceLocation of the line where it is written.
#define RST_SOURCE_LOCATION ::RST::Log::SourceLocation{__FILE__, __func__, __LINE__}
