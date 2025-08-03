#pragma once
#include "imgui.h"

namespace ThemeManager {

// Enumeracja definiująca dostępne motywy.
enum class AppTheme
{
    // --- Dark Themes (Originals) ---
    MIT_Engineering,
    Solarized_Prompter,
    Gruvbox_Retro,
    Monokai_Classic,
    
    // --- Dark Themes (New & Awesome) ---
    Cyberpunk_Neon,
    Synthwave_Sunset,
    Dracula_Official,
    One_Dark_Pro,
    Volcanic_Obsidian,
    Deep_Forest_Gold,
    Galactic_Nebula,
    Royal_Amethyst,
    Matrix_Code,
    Molten_Gold,
    C8bit_Commodore,
    Vaporwave_Dream,
    Atom_Inspired,
    Midnight_Ocean,
    Ruby_Glow,

    // --- Light Themes ---
    Stanford_Cardinal_Light,
    Paper_White,
    Clean_Slate_Blue,
    Nordic_Light,
    Sahara_Cream,
    Kyoto_Sakura,
    Spring_Mint,
    Aqua_Marine,
    Lavender_Haze,
    Peach_Sorbet,
    Sunny_Citrus,
    Harvard_Crimson,
    Oxford_Blue,
    Silverlight,
    Alabaster_Shell,
    Coral_Reef
};

// Zmienne globalne zadeklarowane jako 'extern', aby były dostępne w innych jednostkach kompilacji.
extern AppTheme current_theme;
extern const char* theme_names[]; // Rozmiar zostanie określony w definicji w pliku .cpp
extern const int theme_count;

// Główna funkcja do aplikowania wybranego motywu.
void ApplyTheme(AppTheme theme);

} // namespace ThemeManager
