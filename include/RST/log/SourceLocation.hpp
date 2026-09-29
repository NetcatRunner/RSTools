#pragma once

namespace RST::Log {

    struct SourceLocation {
        const char* file = nullptr;
        const char* func = nullptr;
        int line = 0;

        [[nodiscard]] constexpr bool valid() const noexcept { return file != nullptr; }
    };

}

#define RST_SOURCE_LOCATION ::RST::Log::SourceLocation{__FILE__, __func__, __LINE__}
