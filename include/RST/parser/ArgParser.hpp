#pragma once

#include "RST/parser/Argument.hpp"
#include "RST/parser/ParseResult.hpp"

#include <cstddef>
#include <deque>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RST::Parser {

    /// Command-line parser with flags, options, positionals, subcommands and generated help.
    ///
    /// `-h`/`--help` and `--version` are built in. Values not given on the command line fall back on
    /// their environment variable, then the config file, then their default value.
    /// @code
    /// RST::Parser::ArgParser parser("app", "Does useful things.");
    /// parser.addFlag({"-v", "--verbose"}, "Print more details");
    /// parser.addOption({"-n", "--count"}, "How many times").integer().defaultValue(1);
    ///
    /// if (auto result = parser.parse(argc, argv); !result) {
    ///     result.print();
    ///     return result.exitCode();
    /// }
    /// int count = parser.get<int>("--count").value_or(1);
    /// @endcode
    class ArgParser {
    public:
        /// Creates a parser; `programName` defaults to the executable name.
        explicit ArgParser(std::string programName = "", std::string description = "");

        // ── Declaration ──────────────────────────────────────────────────────
        /// Adds a flag without value; short flags can be bundled, like `-abc`.
        Argument& addFlag(std::vector<std::string> flags, std::string description);
        /// Adds an option taking a value: `--name value`, `--name=value`, `-n value` or `-n5`.
        Argument& addOption(std::vector<std::string> flags, std::string description);
        /// Adds a positional argument; positionals are filled in declaration order.
        Argument& addPositional(std::string name, std::string description);
        /// Adds a subcommand with its own arguments, and returns its parser.
        ArgParser& addSubcommand(std::string name, std::string description);

        /// Enables `--version`, which prints the program name and `version`.
        void setVersion(std::string version) { _version = std::move(version); }
        /// Text printed at the end of the help.
        void setEpilog(std::string text) { _epilog = std::move(text); }

        /// Loads fallback values from a file of `name = value` lines, `name` being a flag without dashes.
        ///
        /// Lines starting with `#` or `;` are comments. Returns false if the file cannot be read.
        bool loadConfig(const std::filesystem::path& file);

        // ── Parsing ──────────────────────────────────────────────────────────
        /// Parses the arguments of `main`.
        [[nodiscard]] ParseResult parse(int argc, const char* const* argv);
        /// Parses `args`, without the program name.
        [[nodiscard]] ParseResult parse(const std::vector<std::string>& args);

        // ── Result ──────────────────────────────────────────────────────────
        /// Shortcut for Argument::isSet(); `name` is a flag (`-v`, `--verbose`) or a name (`verbose`).
        [[nodiscard]] bool isSet(std::string_view name) const { return lookup(name).isSet(); }
        /// Shortcut for Argument::count().
        [[nodiscard]] std::size_t count(std::string_view name) const { return lookup(name).count(); }

        /// Shortcut for Argument::get().
        template <typename T = std::string>
        [[nodiscard]] std::optional<T> get(std::string_view name) const { return lookup(name).get<T>(); }

        /// Shortcut for Argument::getAll().
        template <typename T = std::string>
        [[nodiscard]] std::vector<T> getAll(std::string_view name) const { return lookup(name).getAll<T>(); }

        /// Name of the subcommand given on the command line, or an empty string.
        [[nodiscard]] const std::string& activeSubcommand() const noexcept { return _activeSubcommand; }
        /// Parser of the active subcommand, or `nullptr`.
        [[nodiscard]] const ArgParser* activeSubparser() const;

        // ── Help ─────────────────────────────────────────────────────────────
        /// The generated help text.
        [[nodiscard]] std::string help() const;
        /// Prints help() to `stdout`.
        void printHelp() const;

    private:
        struct Subcommand {
            std::string name;
            std::unique_ptr<ArgParser> parser;
        };

        Argument& addArgument(Argument::Kind kind, std::vector<std::string> flags, std::string description);

        [[nodiscard]] ParseResult parseLongOption(std::string_view token, const std::vector<std::string>& args, std::size_t& index);
        [[nodiscard]] ParseResult parseShortOptions(std::string_view token, const std::vector<std::string>& args, std::size_t& index);
        [[nodiscard]] ParseResult parsePositional(const std::string& token, std::size_t& index);
        [[nodiscard]] ParseResult parseSubcommand(const Subcommand& command, const std::vector<std::string>& args, std::size_t first);
        [[nodiscard]] ParseResult takeValues(Argument& argument, std::string_view flag, std::optional<std::string_view> inlineValue,
                                             const std::vector<std::string>& args, std::size_t& index);
        [[nodiscard]] ParseResult builtInOption(std::string_view flag) const;
        [[nodiscard]] ParseResult applyFallbacks();
        [[nodiscard]] ParseResult checkRequired() const;
        [[nodiscard]] ParseResult fail(const std::string& error) const;

        [[nodiscard]] const Argument& lookup(std::string_view name) const;
        [[nodiscard]] const Argument* findFlag(std::string_view flag) const;
        [[nodiscard]] Argument* findFlag(std::string_view flag);
        [[nodiscard]] Argument* findNegatedFlag(std::string_view flag);
        [[nodiscard]] Argument* positionalAt(std::size_t index);
        [[nodiscard]] const Subcommand* findSubcommand(std::string_view name) const;
        [[nodiscard]] bool isOptionToken(std::string_view token) const;
        [[nodiscard]] std::optional<std::string> configValue(const Argument& argument) const;
        [[nodiscard]] std::vector<std::string> longFlags() const;
        [[nodiscard]] std::string programName() const;
        [[nodiscard]] std::string usage() const;

        std::string _programName;
        std::string _description;
        std::string _version;
        std::string _epilog;
        std::deque<Argument> _arguments;
        std::vector<Subcommand> _subcommands;
        std::unordered_map<std::string, std::string> _config;
        std::string _activeSubcommand;
    };
}
