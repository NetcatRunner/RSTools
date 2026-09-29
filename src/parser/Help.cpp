#include "RST/parser/ArgParser.hpp"

#include <algorithm>
#include <iostream>

namespace RST::Parser {

    namespace {

        struct HelpRow {
            std::string left;
            std::string right;
        };

        void appendSection(std::string& text, std::string_view title, const std::vector<HelpRow>& rows)
        {
            if (rows.empty())
                return;

            std::size_t width = 0;
            for (const HelpRow& row : rows)
                width = std::max(width, row.left.size());
            width = std::min<std::size_t>(width, 30);

            text += "\n" + std::string(title) + ":\n";
            for (const HelpRow& row : rows) {
                text += "  " + row.left;
                if (!row.right.empty()) {
                    const bool tooWide = row.left.size() > width;
                    text += tooWide ? "\n  " + std::string(width, ' ') : std::string(width - row.left.size(), ' ');
                    text += "  " + row.right;
                }
                text += "\n";
            }
        }

    }

    void ParseResult::print() const
    {
        std::ostream& out = status == ParseStatus::Error ? std::cerr : std::cout;
        out << message;
        if (!message.empty() && message.back() != '\n')
            out << '\n';
    }

    std::string ArgParser::help() const
    {
        std::vector<HelpRow> positionals;
        std::vector<HelpRow> options;
        std::vector<HelpRow> commands;

        if (findFlag("--help") == nullptr)
            options.push_back({findFlag("-h") == nullptr ? "-h, --help" : "    --help", "Show this help and exit"});
        if (!_version.empty() && findFlag("--version") == nullptr)
            options.push_back({"    --version", "Show the version and exit"});
        for (const Argument& argument : _arguments)
            (argument.isPositional() ? positionals : options).push_back({argument.helpFlags(), argument.helpDetails()});
        for (const Subcommand& command : _subcommands)
            commands.push_back({command.name, command.parser->_description});

        std::string text = usage() + "\n";
        if (!_description.empty())
            text += "\n" + _description + "\n";
        appendSection(text, "Arguments", positionals);
        appendSection(text, "Options", options);
        appendSection(text, "Commands", commands);
        if (!_subcommands.empty())
            text += "\nRun '" + programName() + " <command> --help' for more on a command.\n";
        if (!_epilog.empty())
            text += "\n" + _epilog + "\n";
        return text;
    }

    void ArgParser::printHelp() const
    {
        std::cout << help();
    }

    std::string ArgParser::usage() const
    {
        std::string text = "Usage: " + programName() + (_subcommands.empty() ? "" : " <command>") + " [options]";
        for (const Argument& argument : _arguments) {
            if (!argument.isPositional())
                continue;
            const std::string name = "<" + argument._name + ">" + (argument.maxValues() > 1 ? "..." : "");
            text += argument._required ? " " + name : " [" + name + "]";
        }
        return text;
    }
}
