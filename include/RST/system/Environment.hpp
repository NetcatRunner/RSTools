#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace RST::System {

    [[nodiscard]] std::optional<std::string> getEnv(std::string_view name);

    bool setEnv(std::string_view name, std::string_view value, bool overwrite = true);

    bool unsetEnv(std::string_view name);
}
