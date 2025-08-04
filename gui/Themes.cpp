#include "Themes.h"
#include "imgui.h"
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <iostream>
#include <vector>
#include <string>

// Definicje struktur przeniesione tutaj, aby uniknąć zbędnych nagłówków
struct ThemeColor {
    ImGuiCol Col;
    ImVec4 Color;
};

struct ThemeStyle {
    ImGuiStyleVar Var;
    ImVec2 Value;
};

struct ThemeDefinition {
    std::string Name;
    std::vector<ThemeColor> Colors;
    std::vector<ThemeStyle> Styles;
};

// --- Funkcje-Fabryki dla Motywów ---

ThemeDefinition CreateMITEngineeringTheme() {
    // POPRAWKA: Używamy inicjalizacji listą, a nie przypisania
    ThemeDefinition theme = {
        "MIT Engineering (Dark)",
        { // Colors
            {ImGuiCol_Text, ImVec4(0.95f, 0.96f, 0.98f, 1.00f)}, {ImGuiCol_TextDisabled, ImVec4(0.36f, 0.42f, 0.47f, 1.00f)},
            {ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.00f)}, {ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.00f)},
            {ImGuiCol_PopupBg, ImVec4(0.08f, 0.08f, 0.08f, 0.94f)}, {ImGuiCol_Border, ImVec4(0.20f, 0.20f, 0.20f, 1.00f)},
            {ImGuiCol_BorderShadow, ImVec4(0.00f, 0.00f, 0.00f, 0.00f)}, {ImGuiCol_FrameBg, ImVec4(0.20f, 0.21f, 0.22f, 0.54f)},
            {ImGuiCol_FrameBgHovered, ImVec4(0.37f, 0.38f, 0.39f, 0.40f)}, {ImGuiCol_FrameBgActive, ImVec4(0.15f, 0.15f, 0.15f, 0.67f)},
            {ImGuiCol_TitleBg, ImVec4(0.04f, 0.04f, 0.04f, 1.00f)}, {ImGuiCol_TitleBgActive, ImVec4(0.29f, 0.29f, 0.29f, 1.00f)},
            {ImGuiCol_TitleBgCollapsed, ImVec4(0.00f, 0.00f, 0.00f, 0.51f)}, {ImGuiCol_MenuBarBg, ImVec4(0.14f, 0.14f, 0.14f, 1.00f)},
            {ImGuiCol_ScrollbarBg, ImVec4(0.02f, 0.02f, 0.02f, 0.53f)}, {ImGuiCol_ScrollbarGrab, ImVec4(0.31f, 0.31f, 0.31f, 1.00f)},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4(0.41f, 0.41f, 0.41f, 1.00f)}, {ImGuiCol_ScrollbarGrabActive, ImVec4(0.51f, 0.51f, 0.51f, 1.00f)},
            {ImGuiCol_CheckMark, ImVec4(0.00f, 0.67f, 0.84f, 1.00f)}, {ImGuiCol_SliderGrab, ImVec4(0.00f, 0.67f, 0.84f, 1.00f)},
            {ImGuiCol_SliderGrabActive, ImVec4(0.00f, 0.50f, 0.63f, 1.00f)}, {ImGuiCol_Button, ImVec4(0.00f, 0.67f, 0.84f, 0.40f)},
            {ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.67f, 0.84f, 1.00f)}, {ImGuiCol_ButtonActive, ImVec4(0.00f, 0.50f, 0.63f, 1.00f)},
            {ImGuiCol_Header, ImVec4(0.20f, 0.21f, 0.22f, 0.54f)}, {ImGuiCol_HeaderHovered, ImVec4(0.00f, 0.67f, 0.84f, 0.80f)},
            {ImGuiCol_HeaderActive, ImVec4(0.00f, 0.67f, 0.84f, 1.00f)}, {ImGuiCol_Separator, ImVec4(0.20f, 0.20f, 0.20f, 1.00f)},
            {ImGuiCol_SeparatorHovered, ImVec4(0.10f, 0.40f, 0.75f, 0.78f)}, {ImGuiCol_SeparatorActive, ImVec4(0.10f, 0.40f, 0.75f, 1.00f)},
            {ImGuiCol_ResizeGrip, ImVec4(0.26f, 0.59f, 0.98f, 0.25f)}, {ImGuiCol_ResizeGripHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f)},
            {ImGuiCol_ResizeGripActive, ImVec4(0.26f, 0.59f, 0.98f, 0.95f)}, {ImGuiCol_Tab, ImVec4(0.18f, 0.35f, 0.58f, 0.86f)},
            {ImGuiCol_TabHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f)}, {ImGuiCol_TabActive, ImVec4(0.20f, 0.41f, 0.68f, 1.00f)},
            {ImGuiCol_TabUnfocused, ImVec4(0.09f, 0.15f, 0.24f, 0.97f)}, {ImGuiCol_TabUnfocusedActive, ImVec4(0.14f, 0.26f, 0.42f, 1.00f)},
            {ImGuiCol_PlotLines, ImVec4(0.61f, 0.61f, 0.61f, 1.00f)}, {ImGuiCol_PlotLinesHovered, ImVec4(1.00f, 0.43f, 0.35f, 1.00f)},
            {ImGuiCol_PlotHistogram, ImVec4(0.90f, 0.70f, 0.00f, 1.00f)}, {ImGuiCol_PlotHistogramHovered, ImVec4(1.00f, 0.60f, 0.00f, 1.00f)},
            {ImGuiCol_TableHeaderBg, ImVec4(0.19f, 0.19f, 0.20f, 1.00f)}, {ImGuiCol_TableBorderStrong, ImVec4(0.31f, 0.31f, 0.35f, 1.00f)},
            {ImGuiCol_TableBorderLight, ImVec4(0.23f, 0.23f, 0.25f, 1.00f)}, {ImGuiCol_TableRowBg, ImVec4(0.00f, 0.00f, 0.00f, 0.00f)},
            {ImGuiCol_TableRowBgAlt, ImVec4(1.00f, 1.00f, 1.00f, 0.06f)}, {ImGuiCol_TextSelectedBg, ImVec4(0.26f, 0.59f, 0.98f, 0.35f)},
            {ImGuiCol_DragDropTarget, ImVec4(1.00f, 1.00f, 0.00f, 0.90f)}, {ImGuiCol_NavHighlight, ImVec4(0.26f, 0.59f, 0.98f, 1.00f)},
            {ImGuiCol_NavWindowingHighlight, ImVec4(1.00f, 1.00f, 1.00f, 0.70f)}, {ImGuiCol_NavWindowingDimBg, ImVec4(0.80f, 0.80f, 0.80f, 0.20f)},
            {ImGuiCol_ModalWindowDimBg, ImVec4(0.80f, 0.80f, 0.80f, 0.35f)},
        },
        { // Styles - POPRAWIONA SEKCJA
            { ImGuiStyleVar_WindowPadding, ImVec2(8, 8) },
            { ImGuiStyleVar_FramePadding, ImVec2(6, 4) },
            { ImGuiStyleVar_ItemSpacing, ImVec2(8, 4) },
            { ImGuiStyleVar_WindowRounding, ImVec2(0.0f, 0.0f) },
            { ImGuiStyleVar_FrameRounding, ImVec2(4.0f, 0.0f) },
            { ImGuiStyleVar_PopupRounding, ImVec2(4.0f, 0.0f) },
            { ImGuiStyleVar_GrabRounding, ImVec2(4.0f, 0.0f) },
        }
    };
    return theme;
}

namespace ThemeManager
{
    void InitializeAndSeedThemes(SQLite::Database& db) {
        try {
            int count = db.execAndGet("SELECT COUNT(*) FROM themes");
            if (count == 0) {
                std::cout << "Baza motywów jest pusta. Wypełnianie danymi..." << std::endl;
                
                std::vector<ThemeDefinition> initial_themes;
                initial_themes.push_back(CreateMITEngineeringTheme());
                // W przyszłości można dodać: initial_themes.push_back(CreateSolarizedTheme()); itd.

                SQLite::Transaction transaction(db);

                SQLite::Statement query_theme(db, "INSERT INTO themes (name) VALUES (?)");
                SQLite::Statement query_color(db, "INSERT INTO theme_colors VALUES (?, ?, ?, ?, ?, ?)");
                SQLite::Statement query_style(db, "INSERT INTO theme_styles VALUES (?, ?, ?, ?)");

                for (const auto& theme_def : initial_themes) {
                    query_theme.bind(1, theme_def.Name);
                    query_theme.exec();
                    long long theme_id_ll = db.getLastInsertRowid();
                    query_theme.reset();
                    
                    int64_t theme_id = static_cast<int64_t>(theme_id_ll);

                    for (const auto& color : theme_def.Colors) {
                        query_color.bind(1, theme_id);
                        query_color.bind(2, color.Col);
                        query_color.bind(3, color.Color.x);
                        query_color.bind(4, color.Color.y);
                        query_color.bind(5, color.Color.z);
                        query_color.bind(6, color.Color.w);
                        query_color.exec();
                        query_color.reset();
                    }

                    for (const auto& style : theme_def.Styles) {
                        query_style.bind(1, theme_id);
                        query_style.bind(2, style.Var);
                        query_style.bind(3, style.Value.x);
                        query_style.bind(4, style.Value.y);
                        query_style.exec();
                        query_style.reset();
                    }
                }
                transaction.commit();
                std::cout << initial_themes.size() << " motywów zostało dodanych do bazy danych." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas inicjalizacji motywów: " << e.what() << std::endl;
        }
    }

    std::vector<std::string> GetThemeNames(SQLite::Database& db) {
        std::vector<std::string> names;
        try {
            SQLite::Statement query(db, "SELECT name FROM themes ORDER BY theme_id");
            while (query.executeStep()) {
                names.push_back(query.getColumn(0).getString());
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas pobierania nazw motywów: " << e.what() << std::endl;
            names.push_back("MIT Engineering (Dark)"); // Fallback
        }
        return names;
    }

    void ApplyTheme(const std::string& themeName, SQLite::Database& db) {
        try {
            ImGui::StyleColorsDark(); 
            ImGuiStyle& style = ImGui::GetStyle();
            
            SQLite::Statement query_id(db, "SELECT theme_id FROM themes WHERE name = ?");
            query_id.bind(1, themeName);
            if (!query_id.executeStep()) return; 
            int theme_id = query_id.getColumn(0);

            SQLite::Statement query_colors(db, "SELECT imgui_col_id, r, g, b, a FROM theme_colors WHERE theme_id = ?");
            query_colors.bind(1, theme_id);
            while (query_colors.executeStep()) {
                int col_id = query_colors.getColumn(0).getInt();
                float r = (float)query_colors.getColumn(1).getDouble();
                float g = (float)query_colors.getColumn(2).getDouble();
                float b = (float)query_colors.getColumn(3).getDouble();
                float a = (float)query_colors.getColumn(4).getDouble();
                if (col_id >= 0 && col_id < ImGuiCol_COUNT) {
                    style.Colors[col_id] = ImVec4(r, g, b, a);
                }
            }

            SQLite::Statement query_styles(db, "SELECT imgui_style_var_id, value_x, value_y FROM theme_styles WHERE theme_id = ?");
            query_styles.bind(1, theme_id);
            while (query_styles.executeStep()) {
                int var_id = query_styles.getColumn(0).getInt();
                float x = (float)query_styles.getColumn(1).getDouble();
                float y = (float)query_styles.getColumn(2).getDouble();
                
                switch ((ImGuiStyleVar)var_id) {
                    case ImGuiStyleVar_WindowPadding: style.WindowPadding = ImVec2(x, y); break;
                    case ImGuiStyleVar_FramePadding:  style.FramePadding  = ImVec2(x, y); break;
                    case ImGuiStyleVar_ItemSpacing:   style.ItemSpacing   = ImVec2(x, y); break;
                    case ImGuiStyleVar_WindowRounding:style.WindowRounding= x; break;
                    case ImGuiStyleVar_FrameRounding: style.FrameRounding = x; break;
                    case ImGuiStyleVar_PopupRounding: style.PopupRounding = x; break;
                    case ImGuiStyleVar_GrabRounding:  style.GrabRounding  = x; break;
                    case ImGuiStyleVar_FrameBorderSize: style.FrameBorderSize = x; break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas stosowania motywu '" << themeName << "': " << e.what() << std::endl;
        }
    }
} // namespace ThemeManager