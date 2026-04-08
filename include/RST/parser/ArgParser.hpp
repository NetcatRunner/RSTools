#pragma once

#include <vector>
#include <string>
#include <exception>
#include <set>
#include <functional>
#include <memory>
#include <unordered_map>
#include <optional>
#include <sstream>

namespace RST::Parser {

    // ─────────────────────────────────────────────────────────────────────────────
    //  Forward declarations
    // ─────────────────────────────────────────────────────────────────────────────
    class ArgParser;

    // ─────────────────────────────────────────────────────────────────────────────
    //  Nargs descriptor
    // ─────────────────────────────────────────────────────────────────────────────
    struct Nargs {
        int  min = 1;
        int  max = 1;   // -1 = unlimited

        static Nargs exactly(int n)           { return {n, n};   }
        static Nargs atLeast(int n)           { return {n, -1};  }
        static Nargs between(int mn, int mx)  { return {mn, mx}; }
        static Nargs zeroOrMore()             { return {0, -1};  }
        static Nargs oneOrMore()              { return {1, -1};  }
        static Nargs optional()               { return {0, 1};   }
    };

    // ─────────────────────────────────────────────────────────────────────────────
    //  Argument descriptor
    // ─────────────────────────────────────────────────────────────────────────────
    struct OptionArgument {
        std::vector<std::string>           flags;
        std::string                        description;
        bool                               isFlag           = false;
        std::vector<std::string>           values;
        std::string                        defaultValue;
        std::string                        envVar;
        std::set<std::string>              choices;
        Nargs                              nargs            = Nargs::exactly(1);
        bool                               negatable        = false;
        bool                               pathMustExist    = false;
        std::set<std::string>              allowedExts;
        std::optional<std::pair<double,double>> numRange;
        std::function<void(const std::string&)> validator;
        bool                               found            = false; // was it set by the user?
    };

    struct PositionalArgument {
        std::string                        name;
        std::string                        description;
        bool                               isRequired  = true;
        std::vector<std::string>           values;
        Nargs                              nargs       = Nargs::exactly(1);
        std::set<std::string>               choices;
        bool                               pathMustExist = false;
        std::set<std::string>               allowedExts;
        std::optional<std::pair<double,double>> numRange;
        std::function<void(const std::string&)> validator;
    };

    struct Subcommand {
        std::string name;
        std::string description;
        std::shared_ptr<ArgParser> parser;
    };

    // ─────────────────────────────────────────────────────────────────────────────
    //  OptionBuilder
    // ─────────────────────────────────────────────────────────────────────────────
    class OptionBuilder {
    private:
        OptionArgument& _ref;
    public:
        OptionBuilder(struct OptionArgument& ref) : _ref(ref) {};
        // Restrict the accepted values to a finite set
        OptionBuilder& choices(std::initializer_list<std::string> c);

        // Accept multiple values  (--ids 1 2 3)
        OptionBuilder& nargs(Nargs n);
        OptionBuilder& nargs(int exactly);

        // Fall back to an environment variable when the flag is absent
        OptionBuilder& envFallback(const std::string& envVar);

        // Generate --no-<flag> automatically (for flags only)
        OptionBuilder& allowNegation();

        // Custom validation callback  (throws std::invalid_argument on failure)
        OptionBuilder& validate(std::function<void(const std::string&)> fn);

        // Numeric range  [lo, hi]
        OptionBuilder& range(double lo, double hi);

        // Path must exist on disk
        OptionBuilder& mustExist();

        // Allowed file extensions  {".json", ".yaml"}
        OptionBuilder& extension(std::initializer_list<std::string> exts);
    };

    // ─────────────────────────────────────────────────────────────────────────────
    //  PositionalBuilder
    // ─────────────────────────────────────────────────────────────────────────────
    class PositionalBuilder {
    private:
        PositionalArgument& _ref;
    public:
        PositionalBuilder(struct PositionalArgument& ref) : _ref(ref) {}
        PositionalBuilder& choices(std::initializer_list<std::string> c);
        PositionalBuilder& nargs(Nargs n);
        PositionalBuilder& nargs(int exactly);
        PositionalBuilder& validate(std::function<void(const std::string&)> fn);
        PositionalBuilder& mustExist();
        PositionalBuilder& extension(std::initializer_list<std::string> exts);
        PositionalBuilder& range(double lo, double hi);
    };

    // ─────────────────────────────────────────────────────────────────────────────
    //  ArgParser
    // ─────────────────────────────────────────────────────────────────────────────
    class ArgParser
    {
    private:
        std::string _programName;
        std::string _description;

        std::vector<OptionArgument> _optionArg;
        std::vector<PositionalArgument> _positionalArg;
        std::vector<Subcommand> _subcommands;
        std::string _activeSubcommand;
        std::unordered_map<std::string, std::string> _configDefaults;
        
        
        OptionArgument&         getOptionArg(const std::string& flag);
        const OptionArgument&   getOptionArg(const std::string& flag) const;

        PositionalArgument&       getPositionalArg(const std::string& name);
        const PositionalArgument& getPositionalArg(const std::string& name) const;

        void validateOptionValue(const OptionArgument& arg, const std::string& val) const;
        void validatePositionalValue(const PositionalArgument& arg, const std::string& val) const;
    
        bool tokenIsPositional(const std::string& token) const;

        void applyFallbacks();
    
        template<typename T>
        static T convertValue(const std::string& value) {
            if (value.empty())
                return T{};
            std::stringstream ss(value);
            T result;
            ss >> result;
            return result;
        }
    
    public:
        ArgParser() = default;
        ArgParser(const std::string& programName, const std::string& description = "");
        ~ArgParser() = default;

        OptionBuilder addOption(const std::vector<std::string>& flags, const std::string& description, const std::string& defaultValue = "");
        OptionBuilder addFlag(const std::vector<std::string>& flags, const std::string& description);
        PositionalBuilder addPositional(const std::string& name, const std::string& description, bool isRequired = true);
        ArgParser& addSubcommand(const std::string& name, const std::string& description);
        
        void loadDefaults(const std::string& configFile);

        void parse(int argc, const char** argv);

        bool isSet(const std::string& flag) const;

        const std::string& activeSubcommand() const { return _activeSubcommand; }
        ArgParser* activeSubparser();
        const ArgParser* activeSubparser() const;
    
        template<typename T = std::string>
        T getOption(const std::string& flag) const {
            return convertValue<T>(getOptionArg(flag).values.empty() ? "" : getOptionArg(flag).values[0]);
        }

        template<typename T = std::string>
        std::vector<T> getOptionAll(const std::string& flag) const {
            std::vector<T> out;
            for (auto& v : getOptionArg(flag).values)
                out.push_back(convertValue<T>(v));
            return out;
        }

        template<typename T = std::string>
        T getPositional(const std::string& name) const {
            auto& arg = getPositionalArg(name);
            return convertValue<T>(arg.values.empty() ? "" : arg.values[0]);
        }

        template<typename T = std::string>
        std::vector<T> getPositionalAll(const std::string& name) const {
            std::vector<T> out;
            for (auto& v : getPositionalArg(name).values)
                out.push_back(convertValue<T>(v));
            return out;
        }

        void printHelp(const std::string& programName = "") const;
    };

    template<>
    inline std::string ArgParser::convertValue<std::string>(const std::string& value) {
        return value;
    }

    template<>
    inline bool ArgParser::convertValue<bool>(const std::string& value) {
        return value == "1" || value == "true" || value == "yes" || value == "on";
    }
}
