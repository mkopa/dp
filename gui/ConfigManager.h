#pragma once

#include "AppState.h"
#include <string>
#include <filesystem> // <-- BRAKUJĄCY INCLUDE

namespace fs = std::filesystem;

namespace ConfigManager {
    // <-- BRAKUJĄCA DEKLARACJA PUBLICZNEJ FUNKCJI
    fs::path GetConfigDirPath();

    void LoadConfig(AppState& state);
    void SaveConfig(const AppState& state);
}