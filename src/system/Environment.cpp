#include "RST/system/Environment.hpp"

#include <cstdlib>

namespace RST::System {

    namespace {

        [[nodiscard]] bool isValidName(std::string_view name) noexcept
        {
            return !name.empty() && name.find('=') == std::string_view::npos && name.find('\0') == std::string_view::npos;
        }

    }

    std::optional<std::string> getEnv(std::string_view name)
    {
        if (!isValidName(name)) {
            return std::nullopt;
        }
        const std::string key(name);
#if defined(_MSC_VER)
        char* value = nullptr;
        std::size_t size = 0;
        if (::_dupenv_s(&value, &size, key.c_str()) != 0 || value == nullptr) {
            return std::nullopt;
        }
        std::string result(value);
        std::free(value);
        return result;
#else
        const char* value = std::getenv(key.c_str());
        if (value == nullptr) {
            return std::nullopt;
        }
        return std::string(value);
#endif
    }

    bool setEnv(std::string_view name, std::string_view value, bool overwrite)
    {
        if (!isValidName(name) || value.find('\0') != std::string_view::npos) {
            return false;
        }
        const std::string key(name);
        const std::string content(value);
#if defined(_WIN32)
        if (!overwrite && getEnv(name).has_value()) {
            return true;
        }
        return ::_putenv_s(key.c_str(), content.c_str()) == 0;
#else
        return ::setenv(key.c_str(), content.c_str(), overwrite ? 1 : 0) == 0;
#endif
    }

    bool unsetEnv(std::string_view name)
    {
        if (!isValidName(name)) {
            return false;
        }
        const std::string key(name);
#if defined(_WIN32)
        return ::_putenv_s(key.c_str(), "") == 0;
#else
        return ::unsetenv(key.c_str()) == 0;
#endif
    }
}
