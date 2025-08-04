#pragma once
#include "Themes.h"

// Struktura przechowująca stan layoutu i konfiguracji
struct AppState {
    ThemeManager::AppTheme theme = ThemeManager::AppTheme::MIT_Engineering;
    
    // Pozycje dwóch splitterów w głównym layoucie
    float library_pane_width = 350.0f;
    float playground_pane_width = 450.0f;
};
