#include <RST/RST.hpp>

#include <filesystem>
#include <string>
#include <vector>

using RST::Parser::ArgParser;
using RST::Parser::Nargs;
using RST::Parser::ParseStatus;

namespace {

    using Args = std::vector<std::string>;
    using Strings = std::vector<std::string>;

    bool contains(const std::string& text, std::string_view part)
    {
        return text.find(part) != std::string::npos;
    }

}

// Every way of writing an option value gives the same result.
TEST_CASE(test_parser_option_forms) {
    for (const Args& args : {Args{"--output", "a.txt"}, Args{"--output=a.txt"}, Args{"-o", "a.txt"}, Args{"-oa.txt"}, Args{"-o=a.txt"}}) {
        ArgParser parser("app");
        parser.addOption({"-o", "--output"}, "Output file");
        CHECK(parser.parse(args).status == ParseStatus::Ok);
        CHECK(parser.get("--output") == "a.txt");
        CHECK(parser.get("-o") == "a.txt");
        CHECK(parser.get("output") == "a.txt");
    }
}

// Grouped short flags, and repeated flags counted (-vvv).
TEST_CASE(test_parser_flags) {
    ArgParser parser("app");
    parser.addFlag({"-v", "--verbose"}, "More output");
    parser.addFlag({"-q", "--quiet"}, "Less output");
    parser.addFlag({"-f", "--force"}, "Force");

    CHECK(parser.parse(Args{"-vvq", "--verbose"}).status == ParseStatus::Ok);
    CHECK(parser.count("--verbose") == 3u);
    CHECK(parser.get<bool>("--quiet") == true);
    CHECK(parser.isSet("-q"));
    CHECK(parser.get<bool>("--force") == false);
    CHECK(!parser.isSet("--force"));
}

TEST_CASE(test_parser_flag_values_and_negation) {
    ArgParser parser("app");
    parser.addFlag({"--color"}, "Colored output").allowNegation();

    CHECK(parser.parse(Args{"--no-color"}).status == ParseStatus::Ok);
    CHECK(parser.get<bool>("--color") == false);
    CHECK(parser.isSet("--color"));

    CHECK(parser.parse(Args{"--color=off"}).status == ParseStatus::Ok);
    CHECK(parser.get<bool>("--color") == false);
    CHECK(parser.parse(Args{"--color=yes"}).status == ParseStatus::Ok);
    CHECK(parser.get<bool>("--color") == true);

    const auto result = parser.parse(Args{"--color=maybe"});
    CHECK(result.status == ParseStatus::Error);
    CHECK(contains(result.message, "expected true or false"));
}

// Positionals fill in order; a list takes everything left; after "--" nothing is an option,
// and a negative number is a value.
TEST_CASE(test_parser_positionals) {
    ArgParser parser("copy");
    parser.addPositional("source", "What to copy");
    parser.addPositional("targets", "Where to copy").nargs(Nargs::oneOrMore());
    parser.addOption({"-n", "--times"}, "Repeat").integer();

    CHECK(parser.parse(Args{"a.txt", "b.txt", "-n", "-3", "c.txt", "--", "-weird"}).status == ParseStatus::Ok);
    CHECK(parser.get("source") == "a.txt");
    CHECK((parser.getAll("targets") == Strings{"b.txt", "c.txt", "-weird"}));
    CHECK(parser.get<int>("--times") == -3);
}

TEST_CASE(test_parser_optional_positional_and_defaults) {
    ArgParser parser("app");
    parser.addPositional("input", "Input file");
    parser.addPositional("output", "Output file").required(false).defaultValue("out.txt");

    CHECK(parser.parse(Args{"in.txt"}).status == ParseStatus::Ok);
    CHECK(parser.get("output") == "out.txt");
    CHECK(!parser.isSet("output"));

    const auto missing = parser.parse(Args{});
    CHECK(missing.status == ParseStatus::Error);
    CHECK(missing.exitCode() == 2);
    CHECK(contains(missing.message, "missing required argument <input>"));
}

// Repeating an option collects every value: get() gives the last one, getAll() all of them.
TEST_CASE(test_parser_repeated_and_multi_value_options) {
    ArgParser parser("app");
    parser.addOption({"-I", "--include"}, "Include directory");
    parser.addOption({"--size"}, "Width and height").integer().nargs(2);
    parser.addOption({"--tags"}, "Tags").nargs(Nargs::oneOrMore());

    CHECK(parser.parse(Args{"-I", "a", "--include=b", "-Ic", "--size", "640", "480", "--tags", "x", "y", "z"}).status == ParseStatus::Ok);
    CHECK((parser.getAll("--include") == Strings{"a", "b", "c"}));
    CHECK(parser.get("--include") == "c");
    CHECK(parser.count("--include") == 3u);
    CHECK((parser.getAll<int>("--size") == std::vector<int>{640, 480}));
    CHECK(parser.getAll("--tags").size() == 3u);
}

// Values are checked while parsing: the error names the argument and what was expected.
TEST_CASE(test_parser_value_checks) {
    ArgParser parser("app");
    parser.addOption({"--count"}, "Count").integer();
    parser.addOption({"--ratio"}, "Ratio").range(0, 1);
    parser.addOption({"--mode"}, "Mode").choices({"fast", "safe"});
    parser.addOption({"--config"}, "Config").extension({".json", ".yaml"});
    parser.addOption({"--even"}, "Even number").integer().validate(
        [](const std::string& value) { return RST::String::parseNumber<int>(value).value_or(1) % 2 == 0; }, "must be even");
    parser.addOption({"--file"}, "Existing file").mustExist();

    const auto error = [&parser](const Args& args) { return parser.parse(args).message; };
    CHECK(contains(error({"--count", "abc"}), "invalid value 'abc' for --count (expected an integer)"));
    CHECK(contains(error({"--ratio", "1.5"}), "(expected 0 to 1)"));
    CHECK(contains(error({"--ratio", "x"}), "(expected a number)"));
    CHECK(contains(error({"--mode", "slow"}), "(choose from fast, safe)"));
    CHECK(contains(error({"--config", "a.txt"}), "(expected a .json, .yaml file)"));
    CHECK(contains(error({"--even", "3"}), "for --even: must be even"));
    CHECK(contains(error({"--file", "/does/not/exist"}), "does not exist"));

    const std::string folder = std::filesystem::temp_directory_path().string();
    CHECK(parser.parse(Args{"--count", "7", "--ratio", "0.5", "--mode", "safe", "--config", "a.yaml", "--even", "4", "--file", folder}).status == ParseStatus::Ok);
    CHECK(parser.get<double>("--ratio") == 0.5);
    CHECK(parser.get<int>("--even") == 4);
}

// Unknown options suggest the closest one, and every error comes with the usage.
TEST_CASE(test_parser_errors) {
    ArgParser parser("app");
    parser.addOption({"-o", "--output"}, "Output file");
    parser.addOption({"--point"}, "A point").nargs(2);
    parser.addOption({"--token"}, "API token").required();

    const auto unknown = parser.parse(Args{"--outptu", "x"});
    CHECK(unknown.status == ParseStatus::Error);
    CHECK(contains(unknown.message, "error: unknown option '--outptu' (did you mean '--output'?)"));
    CHECK(contains(unknown.message, "Usage: app [options]"));
    CHECK(contains(unknown.message, "Run 'app --help' for more information."));

    CHECK(contains(parser.parse(Args{"--output"}).message, "option '--output' expects a value"));
    CHECK(contains(parser.parse(Args{"--point", "1"}).message, "option '--point' expects 2 values"));
    CHECK(contains(parser.parse(Args{"-x"}).message, "unknown option '-x'"));
    CHECK(contains(parser.parse(Args{"extra"}).message, "unexpected argument 'extra'"));
    CHECK(contains(parser.parse(Args{}).message, "missing required option --token"));
    CHECK(parser.parse(Args{"--token", "abc"}).status == ParseStatus::Ok);
}

// --help is built in: the text comes back in the result, nothing is printed or thrown, and it
// wins over missing arguments.
TEST_CASE(test_parser_help) {
    ArgParser parser("app", "Does things.");
    parser.addFlag({"-v", "--verbose"}, "More output");
    parser.addFlag({"--color"}, "Colored output").allowNegation();
    parser.addOption({"-o", "--output"}, "Output file").valueName("FILE").defaultValue("out.txt");
    parser.addOption({"--level"}, "Level").integer().range(1, 5).envFallback("APP_LEVEL");
    parser.addPositional("input", "Input file");
    parser.setEpilog("Example: app -v in.txt");

    const auto result = parser.parse(Args{"--help"});
    CHECK(result.status == ParseStatus::Help);
    CHECK(result.exitCode() == 0);
    CHECK(result.message == parser.help());

    const std::string& help = result.message;
    CHECK(contains(help, "Usage: app [options] <input>\n"));
    CHECK(contains(help, "\nDoes things.\n"));
    CHECK(contains(help, "\n  input  Input file\n"));
    CHECK(contains(help, "\n  -h, --help           Show this help and exit\n"));
    CHECK(contains(help, "\n      --[no-]color     Colored output\n"));
    CHECK(contains(help, "\n  -o, --output <FILE>  Output file (default: out.txt)\n"));
    CHECK(contains(help, "\n      --level <int>    Level (range: 1 to 5) (env: APP_LEVEL)\n"));
    CHECK(contains(help, "\nExample: app -v in.txt\n"));
    CHECK(parser.parse(Args{"-vh"}).status == ParseStatus::Help);
}

TEST_CASE(test_parser_version) {
    ArgParser parser("app");
    parser.setVersion("1.2.3");

    const auto result = parser.parse(Args{"--version"});
    CHECK(result.status == ParseStatus::Version);
    CHECK(result.exitCode() == 0);
    CHECK(result.message == "app 1.2.3\n");
    CHECK(contains(parser.help(), "      --version  Show the version and exit"));
}

// A program can use -h for itself: --help keeps working.
TEST_CASE(test_parser_user_defined_h) {
    ArgParser parser("app");
    parser.addOption({"-h", "--host"}, "Host");

    CHECK(parser.parse(Args{"-h", "localhost"}).status == ParseStatus::Ok);
    CHECK(parser.get("--host") == "localhost");
    CHECK(parser.parse(Args{"--help"}).status == ParseStatus::Help);
    CHECK(contains(parser.help(), "      --help"));
}

// Options not given come from the environment, then the config file, then the default; those
// values are checked like the others.
TEST_CASE(test_parser_env_and_config) {
    const std::filesystem::path file = std::filesystem::temp_directory_path() / "rst_parser_test.ini";
    CHECK(RST::System::writeFile(file, "# comment\n[section]\nlevel = 4\n--name = \"from file\"\nverbose = yes\n"));
    CHECK(RST::System::setEnv("RST_PARSER_TEST_LEVEL", "9"));

    ArgParser parser("app");
    parser.addOption({"--level"}, "Level").integer().envFallback("RST_PARSER_TEST_LEVEL");
    parser.addOption({"--name"}, "Name").defaultValue("nobody");
    parser.addOption({"--mode"}, "Mode").defaultValue("fast");
    parser.addFlag({"-v", "--verbose"}, "More output");
    CHECK(parser.loadConfig(file));
    CHECK(!parser.loadConfig("/does/not/exist.ini"));

    CHECK(parser.parse(Args{}).status == ParseStatus::Ok);
    CHECK(parser.get<int>("--level") == 9);
    CHECK(parser.get("--name") == "from file");
    CHECK(parser.get<bool>("--verbose") == true);
    CHECK(parser.get("--mode") == "fast");
    CHECK(parser.isSet("--level"));
    CHECK(!parser.isSet("--mode"));

    CHECK(parser.parse(Args{"--level", "1"}).status == ParseStatus::Ok);
    CHECK(parser.get<int>("--level") == 1);

    CHECK(RST::System::setEnv("RST_PARSER_TEST_LEVEL", "high"));
    CHECK(contains(parser.parse(Args{}).message, "(expected an integer) (from $RST_PARSER_TEST_LEVEL)"));

    RST::System::unsetEnv("RST_PARSER_TEST_LEVEL");
    std::filesystem::remove(file);
}

// Each command has its own arguments and help; the root options come before the command.
TEST_CASE(test_parser_subcommands) {
    ArgParser parser("git");
    parser.addFlag({"-v", "--verbose"}, "More output");
    ArgParser& commit = parser.addSubcommand("commit", "Record changes");
    commit.addOption({"-m", "--message"}, "Message").required();
    ArgParser& push = parser.addSubcommand("push", "Upload");
    push.addPositional("remote", "Remote name").required(false);

    CHECK(parser.parse(Args{"-v", "commit", "-m", "fix"}).status == ParseStatus::Ok);
    CHECK(parser.activeSubcommand() == "commit");
    CHECK(parser.activeSubparser() == &commit);
    CHECK(parser.isSet("--verbose"));
    CHECK(commit.get("--message") == "fix");

    const auto help = parser.parse(Args{"commit", "--help"});
    CHECK(help.status == ParseStatus::Help);
    CHECK(contains(help.message, "Usage: git commit [options]"));
    CHECK(contains(help.message, "Record changes"));

    const auto missing = parser.parse(Args{"commit"});
    CHECK(contains(missing.message, "missing required option --message"));
    CHECK(contains(missing.message, "Run 'git commit --help'"));

    CHECK(contains(parser.parse(Args{"comit"}).message, "unknown command 'comit' (did you mean 'commit'?)"));
    CHECK(contains(parser.help(), "\nCommands:\n  commit  Record changes\n  push    Upload\n"));
    CHECK(contains(parser.help(), "Run 'git <command> --help' for more on a command."));

    CHECK(parser.parse(Args{}).status == ParseStatus::Ok);
    CHECK(parser.activeSubcommand().empty());
    CHECK(parser.activeSubparser() == nullptr);
}

// The Argument returned at declaration also reads the result: no name to repeat or mistype,
// and it stays valid however many arguments are added after it.
TEST_CASE(test_parser_argument_handles) {
    ArgParser parser("app");
    auto& jobs = parser.addOption({"-j", "--jobs"}, "Parallel jobs").integer().defaultValue(4);
    auto& files = parser.addPositional("files", "Files").nargs(Nargs::zeroOrMore());
    for (int i = 0; i < 50; ++i)
        parser.addFlag({"--flag" + std::to_string(i)}, "Filler");

    CHECK(parser.parse(Args{"a", "b", "-j8"}).status == ParseStatus::Ok);
    CHECK(jobs.get<int>() == 8);
    CHECK(jobs.count() == 1u);
    CHECK((files.getAll() == Strings{"a", "b"}));

    CHECK(parser.parse(Args{}).status == ParseStatus::Ok);
    CHECK(jobs.get<int>() == 4);
    CHECK(!jobs.isSet());
    CHECK(files.getAll().empty());
}

// main()'s argc/argv as they come (char**); the program name is the executable's file name.
TEST_CASE(test_parser_from_main_arguments) {
    ArgParser parser;
    parser.addFlag({"-v"}, "More output");

    char program[] = "/usr/local/bin/tool";
    char flag[] = "-v";
    char* argv[] = {program, flag, nullptr};
    CHECK(parser.parse(2, argv).status == ParseStatus::Ok);
    CHECK(parser.isSet("-v"));
    CHECK(contains(parser.help(), "Usage: tool [options]"));
}
