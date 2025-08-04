#include "ConfigManager.h"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace ConfigManager {

fs::path GetConfigDirPath() {
    fs::path config_path;
    #ifdef _WIN32
        const char* appdata = std::getenv("APPDATA");
        if (appdata) config_path = fs::path(appdata) / "AIPrompter";
    #else
        const char* home = std::getenv("HOME");
        if (home) {
            const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
            if (xdg_config_home) config_path = fs::path(xdg_config_home) / "AIPrompter";
            else config_path = fs::path(home) / ".config" / "AIPrompter";
        }
    #endif

    if (config_path.empty()) {
        config_path = fs::current_path() / ".config/AIPrompter";
    }

    if (!fs::exists(config_path)) {
        fs::create_directories(config_path);
    }
    return config_path;
}

static fs::path GetConfigFilePath() {
    return GetConfigDirPath() / "ai_prompter.conf";
}

void SaveConfig(const AppState& state) {
    fs::path file_path = GetConfigFilePath();
    std::ofstream out(file_path);
    if (out.is_open()) {
        out << "theme_name=" << state.theme_name << "\n";
        out << "library_pane_width=" << state.library_pane_width << "\n";
        out << "playground_pane_width=" << state.playground_pane_width << "\n";
    }
}

void LoadConfig(AppState& state) {
    fs::path file_path = GetConfigFilePath();
    std::ifstream in(file_path);
    if (!in.is_open()) return;

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            try {
                if (key == "theme_name") {
                    state.theme_name = value;
                } else if (key == "library_pane_width") {
                    state.library_pane_width = std::stof(value);
                } else if (key == "playground_pane_width") {
                    state.playground_pane_width = std::stof(value);
                }
            } catch (const std::invalid_argument& e) {}
        }
    }
}

} // namespace ConfigManager