#include <RST/RST.hpp>
#include <memory>

int main() {
    // ── 1. Init ────────────────────
    auto log = RST::Log::Registry::init("MonApp", RST::Log::LogLevel::Debug);

    // ── 2. Logs ───────────────────────────────────────
    log->trace("Ce message ne s'affiche pas (niveau Debug minimum)");
    log->debug("Démarrage du moteur — threads={}", 4);
    log->info("Application démarrée — v{}.{}", 1, 0);
    log->warn("Mémoire disponible faible : {}MB restant", 128);
    log->error("Fichier introuvable : {}", "config.json");

    // ── 3. Macros  ─────────────
    LOG_INFO("Macro LOG_INFO — fichier et ligne capturés automatiquement");
    LOG_WARN("Attention : valeur inattendue {}", 42);
    LOG_ERROR("Erreur critique dans le module '{}'", "Renderer");

    // ── 4. Loggers  ──────────────────────────────────────────
    auto net = RST::Log::Registry::get("Network");
    net->info("Connexion à {}:{}", "192.168.1.1", 8080);
    net->warn("Timeout après {}ms", 5000);

    return 0;
}
