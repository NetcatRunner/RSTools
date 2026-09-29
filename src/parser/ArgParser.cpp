#include "RST/parser/ArgParser.hpp"

#include "RST/string/Trim.hpp"
#include "RST/system/Environment.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>

namespace RST::Parser {

    namespace {

        [[nodiscard]] std::size_t editDistance(std::string_view a, std::string_view b)
        {
            std::vector<std::size_t> previous(b.size() + 1);
            std::vector<std::size_t> current(b.size() + 1);
            for (std::size_t j = 0; j <= b.size(); ++j)
                previous[j] = j;

            for (std::size_t i = 1; i <= a.size(); ++i) {
                current[0] = i;
                for (std::size_t j = 1; j <= b.size(); ++j)
                    current[j] = std::min({previous[j] + 1, current[j - 1] + 1, previous[j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1)});
                std::swap(previous, current);
            }
            return previous[b.size()];
        }

        [[nodiscard]] std::string suggestion(std::string_view unknown, const std::vector<std::string>& candidates)
        {
            const auto closest = std::ranges::min_element(candidates, {}, [unknown](const std::string& candidate) { return editDistance(unknown, candidate); });
            if (closest == candidates.end() || editDistance(unknown, *closest) > 2)
                return "";
            return " (did you mean '" + *closest + "'?)";
        }

        [[nodiscard]] std::string valueCount(const Nargs& nargs)
        {
            if (nargs.min == nargs.max)
                return nargs.min == 1 ? "a value" : std::to_string(nargs.min) + " values";
            return "at least " + std::to_string(nargs.min) + (nargs.min == 1 ? " value" : " values");
        }

    }

    ArgParser::ArgParser(std::string programName, std::string description)
        : _programName(std::move(programName)), _description(std::move(description))
    {}

    // ── Declaration ─────────────────────────────────────────────────────────
    Argument& ArgParser::addFlag(std::vector<std::string> flags, std::string description)
    {
        return addArgument(Argument::Kind::Flag, std::move(flags), std::move(description)).defaultValue("false");
    }

    Argument& ArgParser::addOption(std::vector<std::string> flags, std::string description)
    {
        return addArgument(Argument::Kind::Option, std::move(flags), std::move(description));
    }

    Argument& ArgParser::addPositional(std::string name, std::string description)
    {
        _arguments.push_back(Argument(Argument::Kind::Positional, {}, std::move(name), std::move(description)));
        return _arguments.back();
    }

    ArgParser& ArgParser::addSubcommand(std::string name, std::string description)
    {
        auto parser = std::make_unique<ArgParser>(name, std::move(description));
        ArgParser& subcommand = *parser;
        _subcommands.push_back({std::move(name), std::move(parser)});
        return subcommand;
    }

    Argument& ArgParser::addArgument(Argument::Kind kind, std::vector<std::string> flags, std::string description)
    {
        assert(!flags.empty() && "an option needs at least one flag");
        for ([[maybe_unused]] const std::string& flag : flags)
            assert(flag.size() > 1 && flag[0] == '-' && findFlag(flag) == nullptr && "flags start with '-' and are declared once");

        const auto longFlag = std::ranges::find_if(flags, [](const std::string& flag) { return flag.starts_with("--"); });
        std::string name = longFlag != flags.end() ? longFlag->substr(2) : (flags.empty() ? "" : flags.front().substr(1));

        _arguments.push_back(Argument(kind, std::move(flags), std::move(name), std::move(description)));
        return _arguments.back();
    }

    bool ArgParser::loadConfig(const std::filesystem::path& file)
    {
        std::ifstream stream(file);
        if (!stream)
            return false;

        for (std::string line; std::getline(stream, line);) {
            const std::string_view content = String::trimView(line);
            const std::size_t equal = content.find('=');
            if (equal == std::string_view::npos || content.starts_with('#') || content.starts_with(';'))
                continue;

            std::string_view key = String::trimView(content.substr(0, equal));
            while (key.starts_with('-'))
                key.remove_prefix(1);
            std::string_view value = String::trimView(content.substr(equal + 1));
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                value = value.substr(1, value.size() - 2);
            _config[std::string(key)] = std::string(value);
        }
        return true;
    }

    // ── Parsing ─────────────────────────────────────────────────────────────
    ParseResult ArgParser::parse(int argc, const char* const* argv)
    {
        if (_programName.empty() && argc > 0)
            _programName = std::filesystem::path(argv[0]).filename().string();
        return parse(std::vector<std::string>(argc > 0 ? argv + 1 : argv, argv + argc));
    }

    ParseResult ArgParser::parse(const std::vector<std::string>& args)
    {
        for (Argument& argument : _arguments)
            argument.clear();
        _activeSubcommand.clear();

        std::size_t positionalIndex = 0;
        bool optionsEnded = false;
        bool commandExpected = !_subcommands.empty();

        for (std::size_t i = 0; i < args.size(); ++i) {
            const std::string& token = args[i];
            if (!optionsEnded && token == "--") {
                optionsEnded = true;
                continue;
            }

            if (!optionsEnded && isOptionToken(token)) {
                ParseResult result = token.starts_with("--") ? parseLongOption(token, args, i) : parseShortOptions(token, args, i);
                if (!result)
                    return result;
                continue;
            }

            if (commandExpected && !optionsEnded) {
                if (const Subcommand* command = findSubcommand(token)) {
                    if (ParseResult result = parseSubcommand(*command, args, i + 1); !result)
                        return result;
                    break;
                }
                if (positionalAt(0) == nullptr) {
                    std::vector<std::string> names;
                    std::ranges::transform(_subcommands, std::back_inserter(names), &Subcommand::name);
                    return fail("unknown command '" + token + "'" + suggestion(token, names));
                }
            }
            commandExpected = false;

            if (ParseResult result = parsePositional(token, positionalIndex); !result)
                return result;
        }

        if (ParseResult result = applyFallbacks(); !result)
            return result;
        return checkRequired();
    }

    ParseResult ArgParser::parseLongOption(std::string_view token, const std::vector<std::string>& args, std::size_t& index)
    {
        const std::size_t equal = token.find('=');
        const std::string_view flag = token.substr(0, equal);
        std::optional<std::string_view> inlineValue;
        if (equal != std::string_view::npos)
            inlineValue = token.substr(equal + 1);

        if (ParseResult result = builtInOption(flag); !result)
            return result;

        if (Argument* argument = findFlag(flag)) {
            if (argument->isFlag() && !inlineValue) {
                argument->setFlag(true);
                return {};
            }
            return takeValues(*argument, flag, inlineValue, args, index);
        }
        if (Argument* negated = findNegatedFlag(flag); negated != nullptr && !inlineValue) {
            negated->setFlag(false);
            return {};
        }
        return fail("unknown option '" + std::string(flag) + "'" + suggestion(flag, longFlags()));
    }

    ParseResult ArgParser::parseShortOptions(std::string_view token, const std::vector<std::string>& args, std::size_t& index)
    {
        for (std::size_t i = 1; i < token.size(); ++i) {
            const std::string flag{'-', token[i]};
            if (ParseResult result = builtInOption(flag); !result)
                return result;

            Argument* argument = findFlag(flag);
            if (argument == nullptr)
                return fail("unknown option '" + flag + "'");
            if (argument->isFlag()) {
                argument->setFlag(true);
                continue;
            }

            std::string_view value = token.substr(i + 1);
            if (value.starts_with('='))
                value.remove_prefix(1);
            return takeValues(*argument, flag, value.empty() ? std::nullopt : std::optional(value), args, index);
        }
        return {};
    }

    ParseResult ArgParser::parsePositional(const std::string& token, std::size_t& index)
    {
        Argument* argument = positionalAt(index);
        if (argument == nullptr)
            return fail("unexpected argument '" + token + "'");
        if (std::string error = argument->checkValue(token); !error.empty())
            return fail(error);

        argument->add({token});
        if (argument->isFull())
            ++index;
        return {};
    }

    ParseResult ArgParser::parseSubcommand(const Subcommand& command, const std::vector<std::string>& args, std::size_t first)
    {
        _activeSubcommand = command.name;
        command.parser->_programName = programName() + " " + command.name;
        return command.parser->parse(std::vector<std::string>(args.begin() + static_cast<std::ptrdiff_t>(first), args.end()));
    }

    ParseResult ArgParser::takeValues(Argument& argument, std::string_view flag, std::optional<std::string_view> inlineValue,
                                      const std::vector<std::string>& args, std::size_t& index)
    {
        std::vector<std::string_view> values;
        if (inlineValue)
            values.push_back(*inlineValue);
        while (values.size() < argument.maxValues() && index + 1 < args.size() && args[index + 1] != "--" && !isOptionToken(args[index + 1]))
            values.push_back(args[++index]);

        if (values.size() < argument.minValues())
            return fail("option '" + std::string(flag) + "' expects " + valueCount(argument._nargs));
        for (const std::string_view value : values) {
            if (std::string error = argument.checkValue(std::string(value)); !error.empty())
                return fail(error);
        }
        argument.add(values);
        return {};
    }

    ParseResult ArgParser::builtInOption(std::string_view flag) const
    {
        if ((flag == "-h" || flag == "--help") && findFlag(flag) == nullptr)
            return {ParseStatus::Help, help()};
        if (flag == "--version" && !_version.empty() && findFlag(flag) == nullptr)
            return {ParseStatus::Version, programName() + " " + _version + "\n"};
        return {};
    }

    ParseResult ArgParser::applyFallbacks()
    {
        for (Argument& argument : _arguments) {
            if (argument.isPositional() || argument.isSet())
                continue;

            std::string source = "$" + argument._envVariable;
            std::optional<std::string> value;
            if (!argument._envVariable.empty())
                value = System::getEnv(argument._envVariable);
            if (!value) {
                source = "the config file";
                value = configValue(argument);
            }
            if (!value)
                continue;

            if (std::string error = argument.checkValue(*value); !error.empty())
                return fail(error + " (from " + source + ")");
            argument.setFallback(std::move(*value));
        }
        return {};
    }

    ParseResult ArgParser::checkRequired() const
    {
        for (const Argument& argument : _arguments) {
            if (argument._required && argument._values.empty() && !argument._defaultValue)
                return fail(std::string("missing required ") + (argument.isPositional() ? "argument " : "option ") + argument.displayName());
            if (argument.isPositional() && !argument._values.empty() && argument._values.size() < argument.minValues())
                return fail("argument " + argument.displayName() + " expects " + valueCount(argument._nargs));
        }
        return {};
    }

    ParseResult ArgParser::fail(const std::string& error) const
    {
        return {ParseStatus::Error, "error: " + error + "\n" + usage() + "\nRun '" + programName() + " --help' for more information.\n"};
    }

    // ── Lookups ─────────────────────────────────────────────────────────────
    const Argument& ArgParser::lookup(std::string_view name) const
    {
        const auto found = std::ranges::find_if(_arguments, [name](const Argument& argument) { return argument._name == name || argument.hasFlag(name); });
        if (found != _arguments.end())
            return *found;

        assert(false && "unknown argument name");
        static const Argument none(Argument::Kind::Option, {}, "", "");
        return none;
    }

    const Argument* ArgParser::findFlag(std::string_view flag) const
    {
        const auto found = std::ranges::find_if(_arguments, [flag](const Argument& argument) { return argument.hasFlag(flag); });
        return found != _arguments.end() ? &*found : nullptr;
    }

    Argument* ArgParser::findFlag(std::string_view flag)
    {
        return const_cast<Argument*>(std::as_const(*this).findFlag(flag));
    }

    Argument* ArgParser::findNegatedFlag(std::string_view flag)
    {
        if (!flag.starts_with("--no-"))
            return nullptr;
        Argument* argument = findFlag("--" + std::string(flag.substr(5)));
        return (argument != nullptr && argument->_negatable) ? argument : nullptr;
    }

    Argument* ArgParser::positionalAt(std::size_t index)
    {
        for (Argument& argument : _arguments) {
            if (!argument.isPositional())
                continue;
            if (index == 0)
                return &argument;
            --index;
        }
        return nullptr;
    }

    const ArgParser::Subcommand* ArgParser::findSubcommand(std::string_view name) const
    {
        const auto found = std::ranges::find(_subcommands, name, &Subcommand::name);
        return found != _subcommands.end() ? &*found : nullptr;
    }

    const ArgParser* ArgParser::activeSubparser() const
    {
        const Subcommand* command = findSubcommand(_activeSubcommand);
        return command != nullptr ? command->parser.get() : nullptr;
    }

    bool ArgParser::isOptionToken(std::string_view token) const
    {
        if (token.size() < 2 || token[0] != '-')
            return false;
        return findFlag(token) != nullptr || !String::parseNumber<double>(token);
    }

    std::optional<std::string> ArgParser::configValue(const Argument& argument) const
    {
        for (std::string_view flag : argument._flags) {
            while (flag.starts_with('-'))
                flag.remove_prefix(1);
            if (const auto found = _config.find(std::string(flag)); found != _config.end())
                return found->second;
        }
        return std::nullopt;
    }

    std::vector<std::string> ArgParser::longFlags() const
    {
        std::vector<std::string> flags = {"--help"};
        if (!_version.empty())
            flags.emplace_back("--version");
        for (const Argument& argument : _arguments)
            std::ranges::copy_if(argument._flags, std::back_inserter(flags), [](const std::string& flag) { return flag.starts_with("--"); });
        return flags;
    }

    std::string ArgParser::programName() const
    {
        return _programName.empty() ? "program" : _programName;
    }
}
