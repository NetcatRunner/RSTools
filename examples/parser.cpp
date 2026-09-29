#include <RST/parser/Parser.hpp>

#include <iostream>
#include <string>

// app [-v] build [-j N] [-m debug|release] <targets>...
// app [-v] clean
int main(int argc, char** argv) {
    RST::Parser::ArgParser parser("app", "Builds and cleans projects.");
    parser.setVersion("1.0.0");
    parser.addFlag({"-v", "--verbose"}, "Print every step");

    RST::Parser::ArgParser& build = parser.addSubcommand("build", "Build targets");
    auto& jobs = build.addOption({"-j", "--jobs"}, "Parallel jobs").integer().range(1, 64).defaultValue(4);
    build.addOption({"-m", "--mode"}, "Build mode").choices({"debug", "release"}).defaultValue("debug");
    build.addPositional("targets", "What to build").nargs(RST::Parser::Nargs::oneOrMore());

    parser.addSubcommand("clean", "Remove the build outputs");

    // --help, --version and errors come back in the result, with the text to print
    if (auto result = parser.parse(argc, argv); !result) {
        result.print();
        return result.exitCode();
    }

    if (parser.activeSubcommand() == "build") {
        const std::string mode = build.get("--mode").value_or("debug");
        for (const std::string& target : build.getAll("targets"))
            std::cout << "building " << target << " in " << mode << " with " << jobs.get<int>().value_or(1) << " jobs\n";
    } else if (parser.activeSubcommand() == "clean") {
        std::cout << (parser.isSet("--verbose") ? "cleaning every output\n" : "cleaning\n");
    } else {
        parser.printHelp();
    }
    return 0;
}
