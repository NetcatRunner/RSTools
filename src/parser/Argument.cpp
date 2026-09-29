#include "RST/parser/Argument.hpp"

#include "RST/string/Join.hpp"

#include <algorithm>
#include <filesystem>
#include <system_error>

namespace RST::Parser {

    Argument::Argument(Kind kind, std::vector<std::string> flags, std::string name, std::string description)
        : _kind(kind), _flags(std::move(flags)), _name(std::move(name)), _description(std::move(description)), _required(kind == Kind::Positional)
    {}

    bool Argument::hasFlag(std::string_view flag) const
    {
        return std::ranges::find(_flags, flag) != _flags.end();
    }

    // ── Parse state ─────────────────────────────────────────────────────────
    void Argument::add(const std::vector<std::string_view>& values)
    {
        _values.insert(_values.end(), values.begin(), values.end());
        ++_count;
        _isSet = true;
    }

    void Argument::setFlag(bool value)
    {
        add({value ? "true" : "false"});
    }

    void Argument::setFallback(std::string value)
    {
        _values = {std::move(value)};
        _isSet = true;
    }

    void Argument::clear()
    {
        _values.clear();
        _count = 0;
        _isSet = false;
    }

    // ── Checks and help ─────────────────────────────────────────────────────
    std::string Argument::displayName() const
    {
        if (isPositional())
            return "<" + _name + ">";
        const auto longFlag = std::ranges::find_if(_flags, [](const std::string& flag) { return flag.starts_with("--"); });
        return longFlag != _flags.end() ? *longFlag : _flags.front();
    }

    std::string Argument::checkValue(const std::string& value) const
    {
        const std::string invalid = "invalid value '" + value + "' for " + displayName();

        if (isFlag())
            return String::parseBool(value) ? "" : invalid + " (expected true or false)";
        if (!_choices.empty() && std::ranges::find(_choices, value) == _choices.end())
            return invalid + " (choose from " + String::join(_choices, ", ") + ")";
        if (_valueType == ValueType::Integer && !String::parseNumber<long long>(value))
            return invalid + " (expected an integer)";
        if (_valueType == ValueType::Number || _range) {
            const std::optional<double> number = String::parseNumber<double>(value);
            if (!number)
                return invalid + " (expected a number)";
            if (_range && (*number < _range->first || *number > _range->second))
                return invalid + std::format(" (expected {} to {})", _range->first, _range->second);
        }
        if (std::error_code error; _mustExist && !std::filesystem::exists(value, error))
            return "path '" + value + "' for " + displayName() + " does not exist";
        if (!_extensions.empty() && std::ranges::find(_extensions, std::filesystem::path(value).extension().string()) == _extensions.end())
            return invalid + " (expected a " + String::join(_extensions, ", ") + " file)";
        if (_check && !_check(value))
            return invalid + (_checkMessage.empty() ? "" : ": " + _checkMessage);
        return {};
    }

    std::string Argument::helpFlags() const
    {
        if (isPositional())
            return _name;

        std::string shortFlags;
        std::string longFlags;
        for (const std::string& flag : _flags) {
            const bool isLong = flag.starts_with("--");
            std::string& list = isLong ? longFlags : shortFlags;
            list += (list.empty() ? "" : ", ") + ((isLong && _negatable) ? "--[no-]" + flag.substr(2) : flag);
        }

        std::string text = shortFlags.empty() ? "    " + longFlags : shortFlags + (longFlags.empty() ? "" : ", " + longFlags);
        if (_kind == Kind::Option) {
            const std::string_view type = _valueType == ValueType::Integer ? "int" : (_valueType == ValueType::Number ? "number" : "value");
            text += " <" + (_valueName.empty() ? std::string(type) : _valueName) + ">" + (maxValues() > 1 ? "..." : "");
        }
        return text;
    }

    std::string Argument::helpDetails() const
    {
        std::string text = _description;
        const auto add = [&text](const std::string& detail) { text += (text.empty() ? "(" : " (") + detail + ")"; };

        if (!_choices.empty())
            add("choices: " + String::join(_choices, ", "));
        if (_range)
            add(std::format("range: {} to {}", _range->first, _range->second));
        if (_defaultValue && !isFlag())
            add("default: " + *_defaultValue);
        if (!_envVariable.empty())
            add("env: " + _envVariable);
        if (_required && !isPositional())
            add("required");
        if (!_required && isPositional())
            add("optional");
        return text;
    }
}
