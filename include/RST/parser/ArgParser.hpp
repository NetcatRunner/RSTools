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

    class ArgParser {
    public:
        explicit ArgParser(std::string programName = "", std::string description = "");

        // ── Declaration ──────────────────────────────────────────────────────
        Argument& addFlag(std::vector<std::string> flags, std::string description);
        Argument& addOption(std::vector<std::string> flags, std::string description);
        Argument& addPositional(std::string name, std::string description);
        ArgParser& addSubcommand(std::string name, std::string description);

        void setVersion(std::string version) { _version = std::move(version); }
        void setEpilog(std::string text) { _epilog = std::move(text); }

        bool loadConfig(const std::filesystem::path& file);

        // ── Parsing ──────────────────────────────────────────────────────────
        [[nodiscard]] ParseResult parse(int argc, const char* const* argv);
        [[nodiscard]] ParseResult parse(const std::vector<std::string>& args);

        // ── Result ──────────────────────────────────────────────────────────
        [[nodiscard]] bool isSet(std::string_view name) const { return lookup(name).isSet(); }
        [[nodiscard]] std::size_t count(std::string_view name) const { return lookup(name).count(); }

        template <typename T = std::string>
        [[nodiscard]] std::optional<T> get(std::string_view name) const { return lookup(name).get<T>(); }

        template <typename T = std::string>
        [[nodiscard]] std::vector<T> getAll(std::string_view name) const { return lookup(name).getAll<T>(); }

        [[nodiscard]] const std::string& activeSubcommand() const noexcept { return _activeSubcommand; }
        [[nodiscard]] const ArgParser* activeSubparser() const;

        // ── Help ─────────────────────────────────────────────────────────────
        [[nodiscard]] std::string help() const;
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
