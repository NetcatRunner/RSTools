#include <iostream>

#include "RST/maths/Maths.hpp"
#include "RST/string/String.hpp"
#include "RST/system/System.hpp"
#include "RST/parser/Parser.hpp"

int main(int argc, const char** argv) {
    RST::Parser::ArgParser parser("myapp", "The ultimate argument parser demo");

    // ── Simple flag ──────────────────────────────────────────────────────────
    parser.addFlag({"--verbose", "-v"}, "Enable verbose output")
          .allowNegation();              // enables --no-verbose

    parser.addFlag({"--help", "-h"}, "Affiche l'aide");
    // ── Option with default ──────────────────────────────────────────────────
    parser.addOption({"--port", "-p"}, "Listening port", "8080")
          .range(1, 65535)
          .envFallback("MY_APP_PORT");   // falls back to $MY_APP_PORT

    // ── Choices ─────────────────────────────────────────────────────────────
    parser.addOption({"--mode"}, "Run mode", "auto")
          .choices({"fast", "slow", "auto"});

    // ── Multi-value nargs ────────────────────────────────────────────────────
    parser.addOption({"--ids"}, "List of IDs").nargs(RST::Parser::Nargs::oneOrMore());

    // ── File with extension constraint ───────────────────────────────────────
    parser.addOption({"--config"}, "Config file path")
          .extension({".json", ".yaml", ".toml"})
          .mustExist();

    // ── Custom validator ─────────────────────────────────────────────────────
    parser.addOption({"--tag"}, "Tag name") .validate([](const std::string& v) {
              if (v.size() > 32)
                  throw std::invalid_argument("Tag must be ≤ 32 characters");
          });


    // ── Positional ───────────────────────────────────────────────────────────
    parser.addPositional("input", "Input file", /*required=*/true)
          .extension({".txt", ".csv"});

    parser.addPositional("output", "Output file", /*required=*/false);

    // ── Subcommand ───────────────────────────────────────────────────────────
    auto& buildCmd = parser.addSubcommand("build", "Build the project");
    buildCmd.addFlag({"--release", "-r"}, "Enable release optimisations");
    buildCmd.addOption({"--target"}, "Build target", "x86_64").choices({"x86_64", "arm64", "wasm"});

    auto& runCmd = parser.addSubcommand("run", "Run the project");
    runCmd.addOption({"--port"}, "Override run port", "3000").range(1, 65535);

    // ── Load defaults from file (lowest priority) ────────────────────────────
    parser.loadDefaults("myapp.conf");  // e.g.  port=9090

    // ── Parse ────────────────────────────────────────────────────────────────
    try {
        parser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "[error] " << e.what() << "\n\n";
        parser.printHelp();
        return 1;
    }

    // ── Handle subcommand ────────────────────────────────────────────────────
    if (!parser.activeSubcommand().empty()) {
        std::cout << "Subcommand: " << parser.activeSubcommand() << "\n";
        const auto* sub = parser.activeSubparser();
        if (parser.activeSubcommand() == "build") {
            bool release = sub->getOption<bool>("--release");
            std::string target = sub->getOption<std::string>("--target");
            std::cout << "  release=" << release << "  target=" << target << "\n";
        } else if (parser.activeSubcommand() == "run") {
            int port = sub->getOption<int>("--port");
            std::cout << "  port=" << port << "\n";
        }
        return 0;
    }

    // ── Retrieve values ──────────────────────────────────────────────────────
    bool        verbose = parser.getOption<bool>("--verbose");
    int         port    = parser.getOption<int>("--port");
    std::string mode    = parser.getOption<std::string>("--mode");
    auto        ids     = parser.getOptionAll<int>("--ids");
    std::string input   = parser.getPositional<std::string>("input");

    std::cout << "verbose = " << verbose    << "\n";
    std::cout << "port    = " << port       << "\n";
    std::cout << "mode    = " << mode       << "\n";
    std::cout << "input   = " << input      << "\n";
    std::cout << "ids     = ";
    for (auto id : ids)
        std::cout << id << " ";
    std::cout << "\n";

    // ── Shell completion / man page generation ───────────────────────────────
    // parser.generateBashCompletion(std::cout);
    // parser.generateZshCompletion(std::cout);
    // parser.generateManPage(std::cout);

    return 0;
}

// int main(int argc, char** argv)
// {
//     std::cout << "--- Start RST Librairy tests ---" << std::endl;

//     // RST::Maths::Vec2i vec(1,2);
    
//     // std::cout << vec << std::endl;

//     // std::cout << RST::Maths::Random<float>(1.5f, 5) << std::endl;

//     // std::cout << RST::Maths::RandomChoice<float>(std::vector({1.5f, 3.5f, 5.5f, 0.5f})) << std::endl;

//     std::cout << "\nOS Name : " << RST::System::GetOSName() << "\n";
//     std::cout << "RAM Available : " << RST::System::GetAvailableRAM() << " / "<< RST::System::GetTotalRAM() << " octects\n";
//     std::cout << "Nb Core : " << RST::System::GetCpuCores() << "\n";
//     std::cout << "Total space : " << RST::System::GetTotalDiskSpace() << " octects\n";
//     std::cout << std::endl;

//     try {
//         // 1. Initialisation du parseur
//         RST::Parser::ArgParser parser("MonApp", "Une application de test pour démontrer les capacités de RST::Parser");

//         // 2. Ajout de flags (booléens sans valeur)
//         parser.addFlag({"-v", "--verbose"}, "Active les logs détaillés");
//         parser.addFlag({"-h", "--help"}, "Affiche l'aide de l'application");

//         // 3. Ajout d'options (clé-valeur)
//         parser.addOption({"-p", "--port"}, "Port d'écoute du serveur", "8080")
//               .range(1024, 65535); // Test du builder : range

//         parser.addOption({"-m", "--mode"}, "Mode d'exécution", "dev")
//               .choices({"dev", "prod", "test"}); // Test du builder : choices

//         parser.addOption({"-c", "--config"}, "Fichier de configuration", "config.json")
//               .extension({".json", ".yaml"}); // Test du builder : extensions

//         // 4. Ajout d'arguments positionnels
//         parser.addPositional("input_files", "Liste des fichiers d'entrée à traiter", true)
//               .nargs(RST::Parser::Nargs::oneOrMore()); // Test du builder : Nargs (un ou plusieurs)

//         // 5. Exécution du parsing
//         // On cast argv en const char** pour correspondre à la signature de votre fonction parse()
//         parser.parse(argc, (const char**)argv);

//         // Si le flag help est présent, on affiche l'aide et on quitte
//         if (parser.isSet("--help") || parser.isSet("-h")) {
//             parser.printHelp("MonApp");
//             return 0;
//         }

//         // 6. Récupération des valeurs
//         bool isVerbose = parser.isSet("--verbose") || parser.isSet("-v");
//         int port = parser.getOption<int>("--port");
//         std::string mode = parser.getOption<std::string>("--mode");
//         std::string config = parser.getOption<std::string>("--config");
        
//         // Récupération de l'argument positionnel (qui peut contenir plusieurs valeurs)
//         std::vector<std::string> inputFiles = parser.getPositionalAll<std::string>("input_files");

//         // 7. Affichage des résultats pour vérifier que tout fonctionne
//         std::cout << "========================================\n";
//         std::cout << " Configuration analysée avec succès !\n";
//         std::cout << "========================================\n";
//         std::cout << "Verbose mode  : " << (isVerbose ? "ACTIF" : "INACTIF") << "\n";
//         std::cout << "Port d'écoute : " << port << "\n";
//         std::cout << "Mode          : " << mode << "\n";
//         std::cout << "Fichier config: " << config << "\n";
        
//         std::cout << "Fichiers d'entrée (" << inputFiles.size() << ") :\n";
//         for (size_t i = 0; i < inputFiles.size(); ++i) {
//             std::cout << "  [" << i + 1 << "] " << inputFiles[i] << "\n";
//         }

//     } 
//     catch (const std::exception& e) {
//         // Gestion des erreurs (ex: type invalide, argument manquant, valeur hors range)
//         std::cerr << "ERREUR DE PARSING : " << e.what() << "\n";
//         std::cerr << "Utilisez --help pour voir l'usage correct.\n";
//         return 1;
//     }

//     return 0;
// }

