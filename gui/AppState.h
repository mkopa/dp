#pragma once
#include <string>

// Struktura przechowująca stan layoutu i konfiguracji
struct AppState {
    std::string theme_name = "MIT Engineering (Dark)";
    
    // Pozycje dwóch splitterów w głównym layoucie
    float library_pane_width = 350.0f;
    float playground_pane_width = 450.0f;
};