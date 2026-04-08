#include "RST/parser/ArgParser.hpp"
#include "RST/string/String.hpp"

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace RST::Parser {

    // ─────────────────────────────────────────────────────────────────────────────
    //  OptionBuilder
    // ─────────────────────────────────────────────────────────────────────────────
    OptionBuilder& OptionBuilder::choices(std::initializer_list<std::string> c) {
        _ref.choices = std::set<std::string>(c);
        return *this;
    }
    OptionBuilder& OptionBuilder::nargs(Nargs n) {
        _ref.nargs = n;
        return *this;
    }
    OptionBuilder& OptionBuilder::nargs(int exactly) {
        _ref.nargs = Nargs::exactly(exactly);
        return *this;
    }
    OptionBuilder& OptionBuilder::envFallback(const std::string& envVar) {
        _ref.envVar = envVar;
        return *this;
    }
    OptionBuilder& OptionBuilder::allowNegation() {
        _ref.negatable = true;
        return *this;
    }
    OptionBuilder& OptionBuilder::validate(std::function<void(const std::string&)> fn) {
        _ref.validator = std::move(fn);
        return *this;
    }
    OptionBuilder& OptionBuilder::range(double lo, double hi) {
        _ref.numRange = {lo, hi};
        return *this;
    }
    OptionBuilder& OptionBuilder::mustExist() {
        _ref.pathMustExist = true;
        return *this;
    }
    OptionBuilder& OptionBuilder::extension(std::initializer_list<std::string> exts) {
        _ref.allowedExts = std::set<std::string>(exts);
        return *this;
    }


    // ─────────────────────────────────────────────────────────────────────────────
    //  PositionalBuilder
    // ─────────────────────────────────────────────────────────────────────────────
    PositionalBuilder& PositionalBuilder::choices(std::initializer_list<std::string> c) {
        _ref.choices = std::set<std::string>(c);
        return *this;
    }
    PositionalBuilder& PositionalBuilder::nargs(Nargs n) {
        _ref.nargs = n;
        return *this;
    }
    PositionalBuilder& PositionalBuilder::nargs(int exactly) {
        _ref.nargs = Nargs::exactly(exactly);
        return *this;
    }
    PositionalBuilder& PositionalBuilder::validate(std::function<void(const std::string&)> fn) {
        _ref.validator = std::move(fn);
        return *this;
    }
    PositionalBuilder& PositionalBuilder::mustExist() {
        _ref.pathMustExist = true;
        return *this;
    }
    PositionalBuilder& PositionalBuilder::extension(std::initializer_list<std::string> exts) {
        _ref.allowedExts = std::set<std::string>(exts);
        return *this;
    }
    PositionalBuilder& PositionalBuilder::range(double lo, double hi) {
        _ref.numRange = {lo, hi};
        return *this;
    }

    // ─────────────────────────────────────────────────────────────────────────────
    //  ArgParser
    // ─────────────────────────────────────────────────────────────────────────────
    ArgParser::ArgParser(const std::string& programName, const std::string& description)
        : _programName(programName) , _description(description)
    {}

    OptionBuilder ArgParser::addOption(const std::vector<std::string>& flags, const std::string& description, const std::string& defaultValue) {
        if (flags.empty())
            throw std::invalid_argument("addOption: flags list must not be empty");

        OptionArgument arg;
        arg.flags = flags;
        arg.description = description;
        arg.isFlag = false;
        arg.defaultValue = defaultValue;

        _optionArg.push_back(std::move(arg));
        return OptionBuilder(_optionArg.back());
    }
    
    OptionBuilder ArgParser::addFlag(const std::vector<std::string>& flags, const std::string& description) {
        if (flags.empty())
            throw std::invalid_argument("addFlag: flags list must not be empty");

        OptionArgument arg;
        arg.flags = flags;
        arg.description = description;
        arg.isFlag = true;

        _optionArg.push_back(std::move(arg));
        return OptionBuilder(_optionArg.back());
    }
    
    PositionalBuilder ArgParser::addPositional(const std::string& name, const std::string& description, bool isRequired) {
        if (name.empty())
            throw std::invalid_argument("addPositional: name must not be empty");

        PositionalArgument arg;
        arg.name = name;
        arg.description = description;
        arg.isRequired = isRequired;

        _positionalArg.push_back(std::move(arg));
        return PositionalBuilder(_positionalArg.back());
    }

    ArgParser& ArgParser::addSubcommand(const std::string& name, const std::string& description)
    {
        auto sp = std::make_shared<ArgParser>(name, description);
        _subcommands.push_back({name, description, sp});
        return *sp;
    }

    void ArgParser::loadDefaults(const std::string& configFile) {
        std::ifstream f(configFile);
        if (!f.is_open())
            return;

        std::string line;
        while (std::getline(f, line)) {
            // Strip comments
            auto commentPos = line.find('#');
            if (commentPos != std::string::npos)
                line = line.substr(0, commentPos);
            RST::String::Ltrim(line);
            RST::String::Rtrim(line);

            if (line.empty())
                continue;

            auto eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            std::string key   = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            RST::String::Ltrim(key);
            RST::String::Rtrim(value);
            _configDefaults[key] = value;
        }
    }

    void ArgParser::applyFallbacks() {
        for (auto& opt : _optionArg) {
            if (opt.found)
                continue;

            // 1. Environment variable
            if (!opt.envVar.empty()) {
                const char* envVal = std::getenv(opt.envVar.c_str());
                if (envVal) {
                    opt.values = {std::string(envVal)};
                    opt.found  = true;
                    continue;
                }
            }

            // 2. Config file
            for (auto& flag : opt.flags) {
                auto it = _configDefaults.find(flag);
                if (it != _configDefaults.end()) {
                    opt.values = {it->second};
                    opt.found  = true;
                    break;
                }
            }

            // 3. Hard-coded default
            if (!opt.found && !opt.defaultValue.empty()) {
                opt.values = {opt.defaultValue};
            }
        }
    }

    bool ArgParser::tokenIsPositional(const std::string& token) const {
        if (token.empty())
            return false;
        if (token == "--")
            return false;

        if (token.front() == '-') {
            std::istringstream iss(token);
            double val;
            iss >> val;
            return (iss.eof() && !iss.fail());
        }
        return true;
    }


    OptionArgument& ArgParser::getOptionArg(const std::string& flag) {
        for (auto& arg : _optionArg) {
            if (std::find(arg.flags.begin(), arg.flags.end(), flag) != arg.flags.end()) {
                return arg;
            }
        }
        throw std::invalid_argument("Unknown option : " + flag);
    }

    const OptionArgument& ArgParser::getOptionArg(const std::string& flag) const {
        for (const auto& arg : _optionArg)
            if (std::find(arg.flags.begin(), arg.flags.end(), flag) != arg.flags.end()) {
                return arg;
            }
        throw std::invalid_argument("Unknown option: " + flag);
    }

    PositionalArgument& ArgParser::getPositionalArg(const std::string& name) {
        for (auto& arg : _positionalArg)
            if (arg.name == name) {
                return arg;
            }
        throw std::invalid_argument("Unknown positional: " + name);
    }

    const PositionalArgument& ArgParser::getPositionalArg(const std::string& name) const {
        for (const auto& arg : _positionalArg)
            if (arg.name == name) {
                return arg;
            }
        throw std::invalid_argument("Unknown positional: " + name);
    }

    bool ArgParser::isSet(const std::string& flag) const {
        return getOptionArg(flag).found;
    }

    ArgParser* ArgParser::activeSubparser() {
        for (auto& sub : _subcommands)
            if (sub.name == _activeSubcommand)
                return sub.parser.get();
        return nullptr;
    }
    const ArgParser* ArgParser::activeSubparser() const {
        for (const auto& sub : _subcommands)
            if (sub.name == _activeSubcommand)
                return sub.parser.get();
        return nullptr;
    }

    void ArgParser::validateOptionValue(const OptionArgument& arg, const std::string& val) const
    {
        if (!arg.choices.empty() && arg.choices.find(val) == arg.choices.end()) {
            std::string allowed;
            for (auto& c : arg.choices)
                allowed += c + ", ";
            if (!allowed.empty())
                allowed.erase(allowed.size()-2);
            throw std::invalid_argument("Invalid value '" + val + "' for " + arg.flags[0] +". Allowed: { " + allowed + " }");
        }
        // numeric range
        if (arg.numRange.has_value()) {
            double num;
            std::istringstream iss(val);
            if (!(iss >> num))
                throw std::invalid_argument("Value '" + val + "' for " + arg.flags[0] + " is not a number");
            auto [lo, hi] = *arg.numRange;
            if (num < lo || num > hi)
                throw std::out_of_range("Value " + val + " for " + arg.flags[0] + " is out of range [" + std::to_string(lo) + ", " + std::to_string(hi) + "]");
        }
        // path must exist
        if (arg.pathMustExist && !std::filesystem::exists(val))
            throw std::invalid_argument("Path '" + val + "' does not exist");
        // extensions
        if (!arg.allowedExts.empty()) {
            std::filesystem::path p(val);
            std::string ext = p.extension().string();
            if (arg.allowedExts.find(ext) == arg.allowedExts.end()) {
                std::string allowed;
                for (auto& e : arg.allowedExts)
                    allowed += e + " ";
                throw std::invalid_argument("File '" + val + "' has wrong extension. Allowed: " + allowed);
            }
        }
        // custom validator
        if (arg.validator)
            arg.validator(val);
    }

    void ArgParser::validatePositionalValue(const PositionalArgument& arg, const std::string& val) const
    {
        if (!arg.choices.empty() && arg.choices.find(val) == arg.choices.end()) {
            std::string allowed;
            for (auto& c : arg.choices)
                allowed += c + ", ";
            if (!allowed.empty())
                allowed.erase(allowed.size()-2);
            throw std::invalid_argument("Invalid value '" + val + "' for <" + arg.name + ">. Allowed: { " + allowed + " }");
        }
        if (arg.pathMustExist && !std::filesystem::exists(val))
            throw std::invalid_argument("Path '" + val + "' does not exist");
        if (!arg.allowedExts.empty()) {
            std::filesystem::path p(val);
            std::string ext = p.extension().string();
            if (arg.allowedExts.find(ext) == arg.allowedExts.end()) {
                std::string allowed;
                for (auto& e : arg.allowedExts)
                    allowed += e + " ";
                throw std::invalid_argument("File '" + val + "' has wrong extension. Allowed: " + allowed);
            }
        }
        if (arg.numRange.has_value()) {
            double num;
            std::istringstream iss(val);
            if (!(iss >> num))
                throw std::invalid_argument("Value '" + val + "' for " + arg.name + " is not a number");
            auto [lo, hi] = *arg.numRange;
            if (num < lo || num > hi)
                throw std::out_of_range("Value " + val + " for " + arg.name + " is out of range [" + std::to_string(lo) + ", " + std::to_string(hi) + "]");
        }
        if (arg.validator)
            arg.validator(val);
    }

    void ArgParser::parse(int argc, const char** argv) {
        if (_programName.empty() && argc > 0)
            _programName = argv[0];

        size_t positionalIndex = 0;
        bool endOfOptions = false;
        
        for (int i = 1; i < argc; i++) {
            const std::string token = argv[i];

            if (token == "--help" || token == "-h")
                throw HelpRequested(_programName);

            if (token == "--") {
                endOfOptions = true;
                continue;
            }

            if (!endOfOptions && positionalIndex == 0 && !_subcommands.empty()) {
                for (auto& sub : _subcommands) {
                    if (sub.name == token) {
                        _activeSubcommand = token;
                        sub.parser->parse(argc - i, argv + i);
                        return;
                    }
                }
            }

            if (!endOfOptions && !tokenIsPositional(token) && token.find('=') != std::string::npos) {
                auto eq         = token.find('=');
                std::string key = token.substr(0, eq);
                std::string val = token.substr(eq + 1);

                OptionArgument& arg = getOptionArg(key);
                validateOptionValue(arg, val);
                arg.values.push_back(val);
                arg.found = true;
                continue;
            }

            if (!endOfOptions && token.size() > 5 && token.substr(0, 5) == "--no-") {
                std::string positive = "--" + token.substr(5);
                bool handled = false;
                for (auto& arg : _optionArg) {
                    if (arg.negatable && std::find(arg.flags.begin(), arg.flags.end(), positive) != arg.flags.end()) {
                        arg.values = {"0"};
                        arg.found  = true;
                        handled    = true;
                        break;
                    }
                }
                if (handled)
                    continue;
            }

            if (!endOfOptions && !tokenIsPositional(token)) {
                OptionArgument* argPtr = nullptr;
                try {
                    argPtr = &getOptionArg(token);
                } catch (const std::invalid_argument&) {
                    throw std::invalid_argument("Unknown option: " + token);
                }
                OptionArgument& arg = *argPtr;

                // Flag (boolean)
                if (arg.isFlag) {
                    arg.values = {"1"};
                    arg.found  = true;
                    continue;
                }

                // Collect nargs values
                int collected = 0;
                while (arg.nargs.max == -1 || collected < arg.nargs.max) {
                    if (i + 1 >= argc)
                        break;
                    const std::string next = argv[i + 1];
                    if (!endOfOptions && !tokenIsPositional(next))
                        break;
                    ++i;
                    validateOptionValue(arg, argv[i]);
                    arg.values.push_back(argv[i]);
                    arg.found = true;
                    ++collected;
                }
                if (collected < arg.nargs.min)
                    throw std::invalid_argument("Not enough values for option: " + token + " (need at least " + std::to_string(arg.nargs.min) + ")");
                continue;
            }

            if (positionalIndex >= _positionalArg.size())
                throw std::out_of_range("Unexpected positional argument: " + token);

            PositionalArgument& posArg = _positionalArg[positionalIndex];
            validatePositionalValue(posArg, token);
            posArg.values.push_back(token);

            if (posArg.nargs.max != -1 &&  static_cast<int>(posArg.values.size()) >= posArg.nargs.max)
                positionalIndex++;

        }

        applyFallbacks();

        for (const auto& posArg : _positionalArg) {
            if (posArg.isRequired && posArg.values.empty())
                throw std::invalid_argument("Missing required positional argument: <" + posArg.name + ">");
            if (!posArg.values.empty() && static_cast<int>(posArg.values.size()) < posArg.nargs.min)
                throw std::invalid_argument("Too few values for positional <" + posArg.name + ">" + " (need at least " + std::to_string(posArg.nargs.min) + ")");
        }

        for (const auto& opt : _optionArg) {
            if (!opt.isFlag && opt.found && static_cast<int>(opt.values.size()) < opt.nargs.min)
                throw std::invalid_argument("Too few values for option '" + opt.flags[0] + "' " "(need at least " + std::to_string(opt.nargs.min) + ")");
        }

    }

    void ArgParser::printHelp(const std::string& programName) const {
        const std::string& pname = programName.empty() ? _programName : programName;

        std::cout << "Usage: " << pname;
        if (!_subcommands.empty())
            std::cout << " <command>";
        if (!_optionArg.empty())
            std::cout << " [options]";
        for (const auto& pos : _positionalArg)
            std::cout << " <" << pos.name << ">";
        std::cout << "\n";

        if (!_description.empty())
            std::cout << "\n" << _description << "\n";

        // Subcommands
        if (!_subcommands.empty()) {
            std::cout << "\nCommands:\n";
            for (const auto& sub : _subcommands)
                std::cout << "  " << std::left << std::setw(20) << sub.name << sub.description << "\n";
        }

        // Positionals
        if (!_positionalArg.empty()) {
            std::cout << "\nPositional Arguments:\n";
            for (const auto& pos : _positionalArg) {
                std::cout << "  " << std::left << std::setw(20) << pos.name << pos.description;
                if (pos.isRequired)
                    std::cout << " (required)";
                if (!pos.choices.empty()) {
                    std::cout << " [choices: ";
                    bool first = true;
                    for (auto& c : pos.choices) {
                        if (!first)
                            std::cout << "|";
                        std::cout << c;
                        first=false;
                    }
                    std::cout << "]";
                }
                std::cout << "\n";
            }
        }

        // Options
        if (!_optionArg.empty()) {
            std::cout << "\nOptions:\n";
            for (size_t i = 0; i < _optionArg.size(); ++i) {
                const auto& arg = _optionArg[i];
                std::cout << "  ";
                for (size_t f = 0; f < arg.flags.size(); ++f) {
                    std::cout << arg.flags[f];
                    if (f + 1 < arg.flags.size())
                        std::cout << ", ";
                }
                if (!arg.isFlag)
                    std::cout << " <value>";
                if (arg.negatable)
                    std::cout << " / --no-<flag>";
                std::cout << "\n";
                std::cout << "      " << arg.description;
                if (!arg.defaultValue.empty())
                    std::cout << " (default: " << arg.defaultValue << ")";
                if (!arg.choices.empty()) {
                    std::cout << " [choices: ";
                    bool first = true;
                    for (auto& c : arg.choices) {
                        if (!first) {
                            std::cout << "|";
                        }
                        std::cout << c;
                        first=false;
                    }
                    std::cout << "]";
                }
                if (!arg.envVar.empty())
                    std::cout << " [$" << arg.envVar << "]";
                if (arg.numRange.has_value())
                    std::cout << " [range: " << arg.numRange->first << "-" << arg.numRange->second << "]";
                std::cout << "\n";
            }

        }
    }
}
