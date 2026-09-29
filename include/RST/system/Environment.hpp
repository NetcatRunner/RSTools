#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace RST::System {

    /// @name Environment variables
    /// @{

    /// Value of the variable `name`, or `std::nullopt` if it is not set.
    [[nodiscard]] std::optional<std::string> getEnv(std::string_view name);

    /// Sets `name` to `value`, unless it is already set and `overwrite` is false; returns false on failure.
    bool setEnv(std::string_view name, std::string_view value, bool overwrite = true);

    /// Removes `name` from the environment; returns false on failure.
    bool unsetEnv(std::string_view name);
    /// @}
}
