#pragma once

#include "Themes.h"
#include <string>

// Struktura przechowująca wszystkie ustawienia aplikacji,
// które chcemy zapisywać i wczytywać.
struct AppConfig {
    ThemeManager::AppTheme theme = ThemeManager::AppTheme::MIT_Engineering;
    float left_pane_width = 400.0f;
};

namespace ConfigManager {

    // Wczytuje konfigurację z pliku. Jeśli plik nie istnieje,
    // zwraca domyślne ustawienia.
    void LoadConfig(AppConfig& config);

    // Zapisuje bieżącą konfigurację do pliku.
    void SaveConfig(const AppConfig& config);

} // namespace ConfigManager