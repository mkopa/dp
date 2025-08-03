#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#include <SDL.h>
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Dołączamy nagłówek naszej biblioteki
#include "dirpacker/dirpacker.h"

// --- Zmienne Globalne Stanu Aplikacji ---
static char role_buffer[1024] = "Jesteś ekspertem programistą C++ z 20-letnim doświadczeniem w systemach o niskim opóźnieniu i wysokiej wydajności.";
static char task_buffer[2048] = "Przeanalizuj poniższy kod. Zidentyfikuj potencjalne błędy, 'code smells', oraz zaproponuj refaktoryzację w celu poprawy czytelności i wydajności.";
static char context_buffer[65536] = ""; // Duży bufor na kod
static char constraints_buffer[1024] = "- Nie używaj bibliotek zewnętrznych poza C++17 STL.\n- Zachowaj istniejącą funkcjonalność.\n- Odpowiadaj w języku polskim.";
static char output_format_buffer[1024] = "Zaproponuj zmiany w formacie 'diff'. Każdą sugestię opatrz zwięzłym uzasadnieniem.";
static std::string final_prompt;

// --- Zarządzanie Motywami ---
enum class AppTheme
{
    MIT_Engineering,
    Stanford_Cardinal_Light,
    Solarized_Prompter,
    Gruvbox_Retro,
    Monokai_Classic
};

static const char *theme_names[] = {
    "MIT Engineering (Dark)",
    "Stanford Cardinal (Light)",
    "Solarized Prompter (Dark)",
    "Gruvbox Retro (Dark)",
    "Monokai Classic (Dark)"};

static AppTheme current_theme = AppTheme::MIT_Engineering;

// Prototypy funkcji, które zdefiniujemy poniżej
void ApplyMITEngineeringTheme();
void ApplyStanfordCardinalLightTheme();
void ApplySolarizedPrompterTheme();
void ApplyGruvboxRetroTheme();
void ApplyMonokaiClassicTheme();

// Funkcja-przełącznik
void ApplyTheme(AppTheme theme)
{
    switch (theme)
    {
    case AppTheme::Stanford_Cardinal_Light:
        ApplyStanfordCardinalLightTheme();
        break;
    case AppTheme::Solarized_Prompter:
        ApplySolarizedPrompterTheme();
        break;
    case AppTheme::Gruvbox_Retro:
        ApplyGruvboxRetroTheme();
        break;
    case AppTheme::Monokai_Classic:
        ApplyMonokaiClassicTheme();
        break;
    case AppTheme::MIT_Engineering:
    default:
        ApplyMITEngineeringTheme();
        break;
    }
    current_theme = theme;
}

// --- Definicje Motywów ---

void ApplyMITEngineeringTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    auto &colors = style.Colors;

    // Palette: #252526 (bg), #333333 (elements), #007ACC (accent)
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.38f, 0.39f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.67f, 0.84f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.67f, 0.84f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.50f, 0.63f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.00f, 0.67f, 0.84f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.67f, 0.84f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.50f, 0.63f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.67f, 0.84f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.67f, 0.84f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.15f, 0.24f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowRounding = 0.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.GrabRounding = 4.0f;
}

void ApplyStanfordCardinalLightTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    auto &colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.60f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.74f, 0.74f, 0.74f, 0.70f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.08f, 0.08f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.55f, 0.08f, 0.08f, 0.60f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.55f, 0.08f, 0.08f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.08f, 0.08f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.55f, 0.08f, 0.08f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.55f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.08f, 0.08f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.55f, 0.08f, 0.08f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.55f, 0.08f, 0.08f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.90f, 0.90f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.08f, 0.08f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.55f, 0.08f, 0.08f, 0.35f);

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.GrabRounding = 2.0f;
}

void ApplySolarizedPrompterTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    auto &colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.51f, 0.58f, 0.59f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.43f, 0.44f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.17f, 0.21f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.21f, 0.26f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.21f, 0.26f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.03f, 0.21f, 0.26f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.03f, 0.21f, 0.26f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.05f, 0.27f, 0.33f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.27f, 0.33f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.17f, 0.21f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.03f, 0.21f, 0.26f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.03f, 0.21f, 0.26f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.21f, 0.26f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.05f, 0.27f, 0.33f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.07f, 0.33f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.08f, 0.39f, 0.47f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.54f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.71f, 0.54f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.34f, 0.10f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.56f, 0.54f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.56f, 0.54f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.70f, 0.68f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.16f, 0.56f, 0.54f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.56f, 0.54f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.56f, 0.54f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15f, 0.35f, 0.41f, 0.75f);

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.GrabRounding = 0.0f;
}

void ApplyGruvboxRetroTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    auto &colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.86f, 0.70f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.57f, 0.53f, 0.48f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.24f, 0.22f, 0.21f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.40f, 0.37f, 0.35f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.37f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.49f, 0.45f, 0.42f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.84f, 0.36f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.49f, 0.45f, 0.42f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.57f, 0.53f, 0.48f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.66f, 0.61f, 0.55f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.84f, 0.36f, 0.05f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.84f, 0.36f, 0.05f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.48f, 0.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.84f, 0.36f, 0.05f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.48f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.84f, 0.36f, 0.05f, 0.80f);
    colors[ImGuiCol_Header] = ImVec4(0.41f, 0.62f, 0.41f, 0.50f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.54f, 0.72f, 0.20f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.61f, 0.77f, 0.14f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.49f, 0.45f, 0.42f, 1.00f);

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
}

void ApplyMonokaiClassicTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();
    auto &colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.97f, 0.97f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.16f, 0.13f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.24f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.15f, 0.45f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.21f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.16f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.26f, 0.26f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.98f, 0.15f, 0.45f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.89f, 0.18f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.98f, 0.15f, 0.45f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.89f, 0.18f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.98f, 0.15f, 0.45f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.23f, 0.51f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.14f, 0.42f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.98f, 0.15f, 0.45f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.15f, 0.45f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.98f, 0.15f, 0.45f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.87f, 0.93f, 0.35f);

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.GrabRounding = 4.0f;
}

// --- Funkcje Pomocnicze ---
int EstimateTokens(const char *text)
{
    if (!text)
        return 0;
    return static_cast<int>(strlen(text) / 4);
}

void ThrowSDLError(const std::string &message)
{
    std::string error_message = message + " SDL_Error: " + SDL_GetError();
    throw std::runtime_error(error_message);
}

// --- Funkcja Renderująca UI ---
void RenderUI()
{
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("Główny Kontener", nullptr, window_flags);

    ImGui::SetNextItemWidth(250);
    if (ImGui::Combo("Wybierz Motyw", (int *)&current_theme, theme_names, IM_ARRAYSIZE(theme_names)))
    {
        ApplyTheme(current_theme);
    }
    ImGui::Separator();

    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("GłównyLayout", 3, table_flags))
    {
        ImGui::TableSetupColumn("Konstruktor", ImGuiTableColumnFlags_WidthStretch, 0.30f);
        ImGui::TableSetupColumn("Kontekst", ImGuiTableColumnFlags_WidthStretch, 0.35f);
        ImGui::TableSetupColumn("Wynik", ImGuiTableColumnFlags_WidthStretch, 0.35f);

        // --- Panel 1: Konstruktor Promptu ---
        ImGui::TableNextColumn();
        ImGui::Text("Konstruktor Promptu");
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Rola / Persona", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ZMIANA: Zamiast -FLT_MIN, używamy GetContentRegionAvail().x, aby wymusić zawijanie tekstu.
            ImGui::InputTextMultiline("##Role", role_buffer, sizeof(role_buffer), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight() * 4));
        }
        if (ImGui::CollapsingHeader("Zadanie", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ZMIANA: Zamiast -FLT_MIN, używamy GetContentRegionAvail().x
            ImGui::InputTextMultiline("##Task", task_buffer, sizeof(task_buffer), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight() * 6));
        }
        if (ImGui::CollapsingHeader("Ograniczenia", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ZMIANA: Zamiast -FLT_MIN, używamy GetContentRegionAvail().x
            ImGui::InputTextMultiline("##Constraints", constraints_buffer, sizeof(constraints_buffer), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight() * 4));
        }
        if (ImGui::CollapsingHeader("Format Wyjściowy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ZMIANA: Zamiast -FLT_MIN, używamy GetContentRegionAvail().x
            ImGui::InputTextMultiline("##OutputFormat", output_format_buffer, sizeof(output_format_buffer), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight() * 4));
        }

        // --- Panel 2: Zarządzanie Kontekstem ---
        ImGui::TableNextColumn();
        ImGui::Text("Kontekst (Kod Źródłowy)");
        ImGui::Separator();
        if (ImGui::Button("Wczytaj kontekst z katalogu (dirpacker)", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            // TODO: Zaimplementować okno dialogowe wyboru katalogu
            strcpy(context_buffer, "--- MIEJSCE NA WYNIK DZIAŁANIA DIRPACKER ---\n");
        }
        // ZMIANA: Używamy GetContentRegionAvail().x dla szerokości i -FLT_MIN dla wysokości, aby wypełnić resztę kolumny.
        ImGui::InputTextMultiline("##Context", context_buffer, sizeof(context_buffer), ImVec2(ImGui::GetContentRegionAvail().x, -FLT_MIN));

        // --- Panel 3: Finalny Prompt i Akcje ---
        ImGui::TableNextColumn();
        ImGui::Text("Finalny Prompt");
        ImGui::Separator();

        std::stringstream ss;
        ss << "### Rola:\n"
           << role_buffer << "\n\n";
        ss << "### Zadanie:\n"
           << task_buffer << "\n\n";
        if (strlen(constraints_buffer) > 0)
            ss << "### Ograniczenia:\n"
               << constraints_buffer << "\n\n";
        if (strlen(output_format_buffer) > 0)
            ss << "### Format Wyjściowy:\n"
               << output_format_buffer << "\n\n";
        ss << "### Kontekst (Kod do analizy):\n```\n"
           << context_buffer << "\n```";
        final_prompt = ss.str();

        int token_count = EstimateTokens(final_prompt.c_str());
        ImGui::Text("Szacunkowa liczba tokenów: %d", token_count);
        ImGui::SameLine();
        if (ImGui::Button("Kopiuj"))
        {
            ImGui::SetClipboardText(final_prompt.c_str());
        }
        // ZMIANA: Używamy GetContentRegionAvail().x dla szerokości i -FLT_MIN dla wysokości.
        ImGui::InputTextMultiline("##FinalPrompt", (char *)final_prompt.c_str(), final_prompt.length() + 1, ImVec2(ImGui::GetContentRegionAvail().x, -FLT_MIN), ImGuiInputTextFlags_ReadOnly);

        ImGui::EndTable();
    }
    ImGui::End();
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        ThrowSDLError("Błąd podczas inicjalizacji SDL.");
    }
    const char *glsl_version = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("AI Prompter (Prof. Marcin & Prof. Gemini)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0); // Wyłącz V-Sync, aby pętla zarządzała klatkami

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ApplyTheme(AppTheme::MIT_Engineering);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    char *base_path_ptr = SDL_GetBasePath();
    if (!base_path_ptr)
    {
        std::cerr << "Warning: Could not determine application base path. Using relative path for assets." << std::endl;
        base_path_ptr = SDL_strdup("./");
    }
    std::string font_path = std::string(base_path_ptr) + "assets/fonts/Roboto-Regular.ttf";
    SDL_free(base_path_ptr);

    float base_font_size = 18.0f;
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), base_font_size, nullptr, io.Fonts->GetGlyphRangesDefault());

    const int ACTIVE_FPS = 60;
    const int IDLE_FPS = 4;
    const Uint32 IDLE_TIMEOUT_MS = 500;
    Uint32 last_event_time = SDL_GetTicks();
    bool is_idle = false;

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        bool event_occured = false;
        if (is_idle)
        {
            if (SDL_WaitEventTimeout(&event, 1000 / IDLE_FPS))
                event_occured = true;
        }
        else
        {
            while (SDL_PollEvent(&event))
            {
                event_occured = true;
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }
        }
        if (event_occured)
        {
            last_event_time = SDL_GetTicks();
            if (is_idle)
                is_idle = false;
            if (event.type != 0)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }
        }
        else
        {
            if (!is_idle && (SDL_GetTicks() - last_event_time > IDLE_TIMEOUT_MS))
                is_idle = true;
        }
        int current_target_fps = is_idle ? IDLE_FPS : ACTIVE_FPS;
        const Uint32 frame_duration = 1000 / current_target_fps;
        Uint32 frame_start = SDL_GetTicks();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        RenderUI();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        Uint32 time_elapsed = SDL_GetTicks() - frame_start;
        if (time_elapsed < frame_duration)
        {
            SDL_Delay(frame_duration - time_elapsed);
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}