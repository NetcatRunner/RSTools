#pragma once

#include "RST/string/Convert.hpp"

#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace RST::Parser {

    struct Nargs {
        int min = 1;
        int max = 1;   // -1 = unlimited

        static Nargs exactly(int n)           { return {n, n};   }
        static Nargs atLeast(int n)           { return {n, -1};  }
        static Nargs between(int mn, int mx)  { return {mn, mx}; }
        static Nargs zeroOrMore()             { return {0, -1};  }
        static Nargs oneOrMore()              { return {1, -1};  }
        static Nargs optional()               { return {0, 1};   }
    };


    class Argument {
    public:
        // ── Declaration ──────────────────────────────────────────────────────
        template <typename T>
        Argument& defaultValue(const T& value) { _defaultValue = std::format("{}", value); return *this; }

        Argument& required(bool isRequired = true) { _required = isRequired; return *this; }
        Argument& valueName(std::string name) { _valueName = std::move(name); return *this; }
        Argument& nargs(int exactly) { return nargs(Nargs::exactly(exactly)); }
        Argument& choices(std::vector<std::string> values) { _choices = std::move(values); return *this; }
        Argument& integer() { _valueType = ValueType::Integer; return *this; }
        Argument& number() { _valueType = ValueType::Number; return *this; }
        Argument& range(double min, double max) { _range = {min, max}; return *this; }
        Argument& mustExist() { _mustExist = true; return *this; }
        Argument& extension(std::vector<std::string> extensions) { _extensions = std::move(extensions); return *this; }
        Argument& envFallback(std::string variable) { _envVariable = std::move(variable); return *this; }
        Argument& allowNegation() { _negatable = true; return *this; }

        Argument& nargs(Nargs count)
        {
            _nargs = count;
            _required = _required && count.min > 0;
            return *this;
        }

        Argument& validate(std::function<bool(const std::string&)> check, std::string message = "")
        {
            _check = std::move(check);
            _checkMessage = std::move(message);
            return *this;
        }

        // ── Result ───────────────────────────────────────────────────────────
        [[nodiscard]] bool isSet() const noexcept { return _isSet; }
        [[nodiscard]] std::size_t count() const noexcept { return _count; }

        template <typename T = std::string>
        [[nodiscard]] std::optional<T> get() const
        {
            if (!_values.empty())
                return convert<T>(_values.back());
            return _defaultValue ? convert<T>(*_defaultValue) : std::nullopt;
        }

        template <typename T = std::string>
        [[nodiscard]] std::vector<T> getAll() const
        {
            std::vector<std::string> values = _values;
            if (values.empty() && _defaultValue)
                values.push_back(*_defaultValue);

            std::vector<T> result;
            for (const std::string& value : values) {
                if (std::optional<T> converted = convert<T>(value))
                    result.push_back(std::move(*converted));
            }
            return result;
        }

    private:
        friend class ArgParser;

        enum class Kind : std::uint8_t { Flag, Option, Positional };
        enum class ValueType : std::uint8_t { Text, Integer, Number };

        Argument(Kind kind, std::vector<std::string> flags, std::string name, std::string description);

        template <typename T>
        [[nodiscard]] static std::optional<T> convert(const std::string& value)
        {
            if constexpr (std::is_same_v<T, bool>)
                return String::parseBool(value);
            else if constexpr (std::is_arithmetic_v<T>)
                return String::parseNumber<T>(value);
            else
                return T(value);
        }

        [[nodiscard]] bool isFlag() const noexcept { return _kind == Kind::Flag; }
        [[nodiscard]] bool isPositional() const noexcept { return _kind == Kind::Positional; }
        [[nodiscard]] bool hasFlag(std::string_view flag) const;
        [[nodiscard]] std::size_t minValues() const noexcept { return static_cast<std::size_t>(_nargs.min); }
        [[nodiscard]] std::size_t maxValues() const noexcept { return _nargs.max < 0 ? std::numeric_limits<std::size_t>::max() : static_cast<std::size_t>(_nargs.max); }
        [[nodiscard]] bool isFull() const noexcept { return _values.size() >= maxValues(); }

        void add(const std::vector<std::string_view>& values);
        void setFlag(bool value);
        void setFallback(std::string value);
        void clear();

        [[nodiscard]] std::string displayName() const;
        [[nodiscard]] std::string checkValue(const std::string& value) const;
        [[nodiscard]] std::string helpFlags() const;
        [[nodiscard]] std::string helpDetails() const;

        Kind _kind;
        std::vector<std::string> _flags;
        std::string _name;
        std::string _description;
        std::string _valueName;
        std::optional<std::string> _defaultValue;
        std::string _envVariable;
        std::vector<std::string> _choices;
        std::vector<std::string> _extensions;
        std::optional<std::pair<double, double>> _range;
        std::function<bool(const std::string&)> _check;
        std::string _checkMessage;
        Nargs _nargs;
        ValueType _valueType = ValueType::Text;
        bool _required = false;
        bool _negatable = false;
        bool _mustExist = false;

        std::vector<std::string> _values;
        std::size_t _count = 0;
        bool _isSet = false;
    };
}
