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

    /// How many values an option or positional takes.
    struct Nargs {
        int min = 1;
        int max = 1;   ///< -1 means unlimited.

        static Nargs exactly(int n)           { return {n, n};   }
        static Nargs atLeast(int n)           { return {n, -1};  }
        static Nargs between(int mn, int mx)  { return {mn, mx}; }
        static Nargs zeroOrMore()             { return {0, -1};  }
        static Nargs oneOrMore()              { return {1, -1};  }
        static Nargs optional()               { return {0, 1};   }
    };


    /// A flag, option or positional argument, configured with chained calls.
    ///
    /// Keep the reference returned by the ArgParser to read the value after parsing:
    /// @code
    /// auto& jobs = parser.addOption({"-j", "--jobs"}, "Parallel jobs").integer().range(1, 64).defaultValue(4);
    /// // after parsing
    /// int count = jobs.get<int>().value_or(4);
    /// @endcode
    class Argument {
    public:
        // ── Declaration ──────────────────────────────────────────────────────
        /// Value used when the argument is not given.
        template <typename T>
        Argument& defaultValue(const T& value) { _defaultValue = std::format("{}", value); return *this; }

        /// Makes the argument mandatory.
        Argument& required(bool isRequired = true) { _required = isRequired; return *this; }
        /// Name of the value in the help, such as `FILE`.
        Argument& valueName(std::string name) { _valueName = std::move(name); return *this; }
        /// Takes exactly that many values.
        Argument& nargs(int exactly) { return nargs(Nargs::exactly(exactly)); }
        /// Only accepts one of `values`.
        Argument& choices(std::vector<std::string> values) { _choices = std::move(values); return *this; }
        /// Only accepts integers.
        Argument& integer() { _valueType = ValueType::Integer; return *this; }
        /// Only accepts numbers.
        Argument& number() { _valueType = ValueType::Number; return *this; }
        /// Only accepts numbers from `min` to `max`, inclusive.
        Argument& range(double min, double max) { _range = {min, max}; return *this; }
        /// Only accepts paths that exist.
        Argument& mustExist() { _mustExist = true; return *this; }
        /// Only accepts paths ending with one of `extensions`, such as `.json`.
        Argument& extension(std::vector<std::string> extensions) { _extensions = std::move(extensions); return *this; }
        /// Reads the environment variable `variable` when the argument is not given.
        Argument& envFallback(std::string variable) { _envVariable = std::move(variable); return *this; }
        /// Lets `--no-name` unset a flag declared as `--name`.
        Argument& allowNegation() { _negatable = true; return *this; }

        /// How many values to take, such as `Nargs::oneOrMore()`.
        Argument& nargs(Nargs count)
        {
            _nargs = count;
            _required = _required && count.min > 0;
            return *this;
        }

        /// Custom check: values for which `check` returns false are rejected with `message`.
        Argument& validate(std::function<bool(const std::string&)> check, std::string message = "")
        {
            _check = std::move(check);
            _checkMessage = std::move(message);
            return *this;
        }

        // ── Result ───────────────────────────────────────────────────────────
        /// Whether a value came from the command line, the environment or the config file.
        [[nodiscard]] bool isSet() const noexcept { return _isSet; }
        /// How many times the argument was given, such as 3 for `-vvv`.
        [[nodiscard]] std::size_t count() const noexcept { return _count; }

        /// The last value, or the default value, converted to `T`; `std::nullopt` if there is none or it does not convert.
        template <typename T = std::string>
        [[nodiscard]] std::optional<T> get() const
        {
            if (!_values.empty())
                return convert<T>(_values.back());
            return _defaultValue ? convert<T>(*_defaultValue) : std::nullopt;
        }

        /// Every value converted to `T`, skipping those that do not convert, or the default value.
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
