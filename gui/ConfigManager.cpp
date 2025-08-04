#include "ConfigManager.h"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib> // Dla getenv

namespace fs = std::filesystem;

namespace ConfigManager {

// Funkcja pomocnicza do znalezienia odpowiedniej ścieżki na plik konfiguracyjny
// w zależności od systemu operacyjnego.
static fs::path GetConfigFilePath() {
    fs::path config_path;
    #ifdef _WIN32
        const char* appdata = std::getenv("APPDATA");
        if (appdata) {
            config_path = fs::path(appdata) / "AIPrompter";
        }
    #else // Dla Linux, macOS i innych systemów uniksowych
        const char* home = std::getenv("HOME");
        if (home) {
            // Preferujemy standard XDG
            const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
            if (xdg_config_home) {
                config_path = fs::path(xdg_config_home) / "AIPrompter";
            } else {
                config_path = fs::path(home) / ".config" / "AIPrompter";
            }
        }
    #endif

    if (config_path.empty()) {
        // Fallback do bieżącego katalogu, jeśli nie udało się znaleźć katalogu domowego
        config_path = "./.config";
    }

    // Utwórz katalog, jeśli nie istnieje
    if (!fs::exists(config_path)) {
        fs::create_directories(config_path);
    }

    return config_path / "ai_prompter.conf";
}

void SaveConfig(const AppConfig& config) {
    fs::path file_path = GetConfigFilePath();
    std::ofstream out(file_path);
    if (out.is_open()) {
        out << "theme=" << static_cast<int>(config.theme) << "\n";
        out << "left_pane_width=" << config.left_pane_width << "\n";
    }
}

void LoadConfig(AppConfig& config) {
    fs::path file_path = GetConfigFilePath();
    std::ifstream in(file_path);
    if (!in.is_open()) {
        return; // Plik nie istnieje, użyj domyślnych
    }

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            if (key == "theme") {
                try {
                    int theme_id = std::stoi(value);
                    if (theme_id >= 0 && theme_id < ThemeManager::theme_count) {
                        config.theme = static_cast<ThemeManager::AppTheme>(theme_id);
                    }
                } catch (...) {}
            } else if (key == "left_pane_width") {
                try {
                    config.left_pane_width = std::stof(value);
                } catch (...) {}
            }
        }
    }
}

} // namespace ConfigManager