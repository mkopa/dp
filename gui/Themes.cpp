#include "Themes.h"
#include "imgui.h"
// Dołączamy imgui_internal.h, aby móc użyć makra IM_ARRAYSIZE, które jest bezpieczniejsze
// niż ręczne obliczenia. Jest to standardowa praktyka wewnątrz implementacji ImGui.
#include "imgui_internal.h"
namespace ThemeManager {
// --- Definicje zmiennych globalnych ---
AppTheme current_theme = AppTheme::MIT_Engineering;
const char* theme_names[] = {
// Ciemne
"MIT Engineering (Dark)",
"Solarized Prompter (Dark)",
"Gruvbox Retro (Dark)",
"Monokai Classic (Dark)",
"Cyberpunk Neon (Dark)",
"Synthwave Sunset (Dark)",
"Dracula Official (Dark)",
"One Dark Pro (Dark)",
"Volcanic Obsidian (Dark)",
"Deep Forest Gold (Dark)",
"Galactic Nebula (Dark)",
"Royal Amethyst (Dark)",
"Matrix Code (Dark)",
"Molten Gold (Dark)",
"8-bit Commodore (Dark)",
"Vaporwave Dream (Dark)",
"Atom Inspired (Dark)",
"Midnight Ocean (Dark)",
"Ruby Glow (Dark)",
// Jasne
"Stanford Cardinal (Light)",
"Paper White (Light)",
"Clean Slate Blue (Light)",
"Nordic Light (Light)",
"Sahara Cream (Light)",
"Kyoto Sakura (Light)",
"Spring Mint (Light)",
"Aqua Marine (Light)",
"Lavender Haze (Light)",
"Peach Sorbet (Light)",
"Sunny Citrus (Light)",
"Harvard Crimson (Light)",
"Oxford Blue (Light)",
"Silverlight (Light)",
"Alabaster Shell (Light)",
"Coral Reef (Light)"
};
const int theme_count = IM_ARRAYSIZE(theme_names);
// --- Prototypy wewnętrznych funkcji ---
static void ApplyMITEngineeringTheme();
static void ApplySolarizedPrompterTheme();
static void ApplyGruvboxRetroTheme();
static void ApplyMonokaiClassicTheme();
static void ApplyCyberpunkNeonTheme();
static void ApplySynthwaveSunsetTheme();
static void ApplyDraculaOfficialTheme();
static void ApplyOneDarkProTheme();
static void ApplyVolcanicObsidianTheme();
static void ApplyDeepForestGoldTheme();
static void ApplyGalacticNebulaTheme();
static void ApplyRoyalAmethystTheme();
static void ApplyMatrixCodeTheme();
static void ApplyMoltenGoldTheme();
static void Apply8bitCommodoreTheme();
static void ApplyVaporwaveDreamTheme();
static void ApplyAtomInspiredTheme();
static void ApplyMidnightOceanTheme();
static void ApplyRubyGlowTheme();
static void ApplyStanfordCardinalLightTheme();
static void ApplyPaperWhiteTheme();
static void ApplyCleanSlateBlueTheme();
static void ApplyNordicLightTheme();
static void ApplySaharaCreamTheme();
static void ApplyKyotoSakuraTheme();
static void ApplySpringMintTheme();
static void ApplyAquaMarineTheme();
static void ApplyLavenderHazeTheme();
static void ApplyPeachSorbetTheme();
static void ApplySunnyCitrusTheme();
static void ApplyHarvardCrimsonTheme();
static void ApplyOxfordBlueTheme();
static void ApplySilverlightTheme();
static void ApplyAlabasterShellTheme();
static void ApplyCoralReefTheme();
// --- Główna funkcja-przełącznik ---
void ApplyTheme(AppTheme theme)
{
switch (theme)
{
case AppTheme::MIT_Engineering: ApplyMITEngineeringTheme(); break;
case AppTheme::Solarized_Prompter: ApplySolarizedPrompterTheme(); break;
case AppTheme::Gruvbox_Retro: ApplyGruvboxRetroTheme(); break;
case AppTheme::Monokai_Classic: ApplyMonokaiClassicTheme(); break;
case AppTheme::Cyberpunk_Neon: ApplyCyberpunkNeonTheme(); break;
case AppTheme::Synthwave_Sunset: ApplySynthwaveSunsetTheme(); break;
case AppTheme::Dracula_Official: ApplyDraculaOfficialTheme(); break;
case AppTheme::One_Dark_Pro: ApplyOneDarkProTheme(); break;
case AppTheme::Volcanic_Obsidian: ApplyVolcanicObsidianTheme(); break;
case AppTheme::Deep_Forest_Gold: ApplyDeepForestGoldTheme(); break;
case AppTheme::Galactic_Nebula: ApplyGalacticNebulaTheme(); break;
case AppTheme::Royal_Amethyst: ApplyRoyalAmethystTheme(); break;
case AppTheme::Matrix_Code: ApplyMatrixCodeTheme(); break;
case AppTheme::Molten_Gold: ApplyMoltenGoldTheme(); break;
case AppTheme::C8bit_Commodore: Apply8bitCommodoreTheme(); break;
case AppTheme::Vaporwave_Dream: ApplyVaporwaveDreamTheme(); break;
case AppTheme::Atom_Inspired: ApplyAtomInspiredTheme(); break;
case AppTheme::Midnight_Ocean: ApplyMidnightOceanTheme(); break;
case AppTheme::Ruby_Glow: ApplyRubyGlowTheme(); break;
case AppTheme::Stanford_Cardinal_Light: ApplyStanfordCardinalLightTheme(); break;
case AppTheme::Paper_White: ApplyPaperWhiteTheme(); break;
case AppTheme::Clean_Slate_Blue: ApplyCleanSlateBlueTheme(); break;
case AppTheme::Nordic_Light: ApplyNordicLightTheme(); break;
case AppTheme::Sahara_Cream: ApplySaharaCreamTheme(); break;
case AppTheme::Kyoto_Sakura: ApplyKyotoSakuraTheme(); break;
case AppTheme::Spring_Mint: ApplySpringMintTheme(); break;
case AppTheme::Aqua_Marine: ApplyAquaMarineTheme(); break;
case AppTheme::Lavender_Haze: ApplyLavenderHazeTheme(); break;
case AppTheme::Peach_Sorbet: ApplyPeachSorbetTheme(); break;
case AppTheme::Sunny_Citrus: ApplySunnyCitrusTheme(); break;
case AppTheme::Harvard_Crimson: ApplyHarvardCrimsonTheme(); break;
case AppTheme::Oxford_Blue: ApplyOxfordBlueTheme(); break;
case AppTheme::Silverlight: ApplySilverlightTheme(); break;
case AppTheme::Alabaster_Shell: ApplyAlabasterShellTheme(); break;
case AppTheme::Coral_Reef: ApplyCoralReefTheme(); break;
    
default: ApplyMITEngineeringTheme(); break;
}
current_theme = theme;

}
// --- Definicje implementacji poszczególnych motywów ---
// --- CIEMNE MOTYWY (Oryginalne) ---
static void ApplyMITEngineeringTheme()
{
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
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
static void ApplySolarizedPrompterTheme()
{
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
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
static void ApplyGruvboxRetroTheme()
{
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
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
static void ApplyMonokaiClassicTheme()
{
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
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
// --- CIEMNE MOTYWY (NOWE I WYSTRZAŁOWE) ---
static void ApplyCyberpunkNeonTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 cyan = ImVec4(0.00f, 1.00f, 1.00f, 1.00f); // #00FFFF
ImVec4 magenta = ImVec4(1.00f, 0.00f, 1.00f, 1.00f); // #FF00FF
ImVec4 bg = ImVec4(0.05f, 0.05f, 0.10f, 1.00f); // #0D0D1A
colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
colors[ImGuiCol_WindowBg] = bg;
colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.15f, 0.94f);
colors[ImGuiCol_Border] = cyan;
colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.15f, 0.54f);
colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.3f, 0.40f);
colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.2f, 0.67f);
colors[ImGuiCol_TitleBgActive] = magenta;
colors[ImGuiCol_CheckMark] = cyan;
colors[ImGuiCol_SliderGrab] = magenta;
colors[ImGuiCol_SliderGrabActive] = cyan;
colors[ImGuiCol_Button] = magenta;
colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.20f, 1.00f, 1.00f);
colors[ImGuiCol_ButtonActive] = cyan;
colors[ImGuiCol_Header] = ImVec4(magenta.x, magenta.y, magenta.z, 0.31f);
colors[ImGuiCol_HeaderHovered] = ImVec4(magenta.x, magenta.y, magenta.z, 0.80f);
colors[ImGuiCol_HeaderActive] = ImVec4(cyan.x, cyan.y, cyan.z, 1.00f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(cyan.x, cyan.y, cyan.z, 0.35f);
style.WindowRounding = 0.0f;
style.FrameRounding = 0.0f;
style.FrameBorderSize = 1.0f;
style.PopupBorderSize = 1.0f;
}
static void ApplySynthwaveSunsetTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 pink = ImVec4(1.00f, 0.22f, 0.83f, 1.00f); // #FF38D4
ImVec4 purple = ImVec4(0.53f, 0.18f, 1.00f, 1.00f); // #872FFF
ImVec4 bg = ImVec4(0.1f, 0.05f, 0.15f, 1.00f); // #1A0D26
colors[ImGuiCol_Text] = ImVec4(0.95f, 0.90f, 1.00f, 1.00f);
colors[ImGuiCol_WindowBg] = bg;
colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.1f, 0.3f, 0.54f);
colors[ImGuiCol_TitleBgActive] = purple;
colors[ImGuiCol_CheckMark] = pink;
colors[ImGuiCol_SliderGrab] = pink;
colors[ImGuiCol_Button] = purple;
colors[ImGuiCol_ButtonHovered] = ImVec4(0.63f, 0.28f, 1.00f, 1.00f);
colors[ImGuiCol_ButtonActive] = pink;
colors[ImGuiCol_Header] = ImVec4(purple.x, purple.y, purple.z, 0.31f);
colors[ImGuiCol_HeaderHovered] = ImVec4(pink.x, pink.y, pink.z, 0.80f);
colors[ImGuiCol_HeaderActive] = pink;
colors[ImGuiCol_Border] = purple;
colors[ImGuiCol_TextSelectedBg] = ImVec4(pink.x, pink.y, pink.z, 0.35f);
}
static void ApplyDraculaOfficialTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f); // F8F8F2
colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f); // 282A36
colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.27f, 0.35f, 0.54f); // 44475A
colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.48f, 0.40f);
colors[ImGuiCol_FrameBgActive] = ImVec4(0.38f, 0.38f, 0.48f, 0.67f);
colors[ImGuiCol_TitleBgActive] = ImVec4(0.38f, 0.61f, 0.93f, 1.00f); // 6272A4
colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.94f, 0.60f, 1.00f); // 50FA97
colors[ImGuiCol_SliderGrab] = ImVec4(0.95f, 0.47f, 0.67f, 1.00f); // FF79C6
colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.47f, 0.67f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(0.38f, 0.61f, 0.93f, 0.40f); // 6272A4
colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.61f, 0.93f, 1.00f);
colors[ImGuiCol_ButtonActive] = ImVec4(0.43f, 0.66f, 0.98f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(0.95f, 0.47f, 0.67f, 0.31f); // FF79C6
colors[ImGuiCol_HeaderHovered] = ImVec4(0.95f, 0.47f, 0.67f, 0.80f);
colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.47f, 0.67f, 1.00f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.31f, 0.94f, 0.60f, 0.35f);
}
static void ApplyOneDarkProTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.70f, 0.73f, 0.78f, 1.00f); // ABB2BF
colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.18f, 0.22f, 1.00f); // 2B2D37
colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f); // 21232B
colors[ImGuiCol_TitleBgActive] = ImVec4(0.38f, 0.62f, 0.89f, 1.00f); // 619EE4
colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.78f, 0.47f, 1.00f); // 98C379
colors[ImGuiCol_SliderGrab] = ImVec4(0.38f, 0.62f, 0.89f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(0.38f, 0.62f, 0.89f, 0.40f);
colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.62f, 0.89f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(0.86f, 0.49f, 0.54f, 0.31f); // E06C75
colors[ImGuiCol_HeaderHovered] = ImVec4(0.86f, 0.49f, 0.54f, 0.80f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.38f, 0.62f, 0.89f, 0.35f);
}
static void ApplyVolcanicObsidianTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.91f, 0.91f, 0.91f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.27f, 0.00f, 1.00f); // FF4500
colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.35f, 0.00f, 1.00f);
colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.35f, 0.00f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(1.00f, 0.27f, 0.00f, 0.60f);
colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.35f, 0.00f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(1.00f, 0.27f, 0.00f, 0.31f);
colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.35f, 0.00f, 0.80f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.27f, 0.00f, 0.35f);
}
static void ApplyDeepForestGoldTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 gold = ImVec4(0.82f, 0.69f, 0.22f, 1.00f); // D4B037
colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.12f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.20f, 0.18f, 1.00f);
colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.3f, 0.05f, 1.00f);
colors[ImGuiCol_CheckMark] = gold;
colors[ImGuiCol_SliderGrab] = gold;
colors[ImGuiCol_Button] = gold;
colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.79f, 0.32f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(gold.x, gold.y, gold.z, 0.31f);
colors[ImGuiCol_HeaderHovered] = ImVec4(gold.x, gold.y, gold.z, 0.80f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(gold.x, gold.y, gold.z, 0.35f);
colors[ImGuiCol_Border] = gold;
}
static void ApplyGalacticNebulaTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent1 = ImVec4(0.61f, 0.35f, 0.71f, 1.00f); // #9C59B6
ImVec4 accent2 = ImVec4(0.20f, 0.67f, 0.83f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.95f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent1;
colors[ImGuiCol_CheckMark] = accent2;
colors[ImGuiCol_SliderGrab] = accent2;
colors[ImGuiCol_Button] = accent1;
colors[ImGuiCol_ButtonHovered] = accent2;
colors[ImGuiCol_Header] = accent1;
colors[ImGuiCol_HeaderHovered] = accent2;
colors[ImGuiCol_TextSelectedBg] = accent2;
}
static void ApplyRoyalAmethystTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 purple = ImVec4(0.55f, 0.33f, 0.83f, 1.00f); // #8E54D4
ImVec4 dark_bg = ImVec4(0.18f, 0.15f, 0.21f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.95f, 0.93f, 0.98f, 1.00f);
colors[ImGuiCol_WindowBg] = dark_bg;
colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.22f, 0.28f, 1.00f);
colors[ImGuiCol_TitleBgActive] = purple;
colors[ImGuiCol_CheckMark] = purple;
colors[ImGuiCol_SliderGrab] = purple;
colors[ImGuiCol_Button] = purple;
colors[ImGuiCol_ButtonHovered] = ImVec4(0.65f, 0.43f, 0.93f, 1.00f);
colors[ImGuiCol_Header] = purple;
colors[ImGuiCol_TextSelectedBg] = ImVec4(purple.x, purple.y, purple.z, 0.4f);
}
static void ApplyMatrixCodeTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 green = ImVec4(0.1f, 1.0f, 0.2f, 1.00f);
colors[ImGuiCol_Text] = green;
colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.05f, 0.0f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.1f, 0.0f, 1.00f);
colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.2f, 0.0f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(0.05f, 0.3f, 0.05f, 1.00f);
colors[ImGuiCol_ButtonHovered] = ImVec4(0.1f, 0.5f, 0.1f, 1.00f);
colors[ImGuiCol_Header] = colors[ImGuiCol_Button];
colors[ImGuiCol_HeaderHovered] = colors[ImGuiCol_ButtonHovered];
colors[ImGuiCol_CheckMark] = green;
colors[ImGuiCol_SliderGrab] = green;
colors[ImGuiCol_Border] = green;
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.05f, 0.4f, 0.05f, 1.00f);
}
static void ApplyMoltenGoldTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 gold = ImVec4(1.00f, 0.84f, 0.00f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
colors[ImGuiCol_CheckMark] = gold;
colors[ImGuiCol_SliderGrab] = gold;
colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
colors[ImGuiCol_Border] = gold;
colors[ImGuiCol_TextSelectedBg] = ImVec4(gold.x, gold.y, gold.z, 0.3f);
}
static void Apply8bitCommodoreTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.44f, 0.69f, 0.83f, 1.00f); // Light Blue
colors[ImGuiCol_WindowBg] = ImVec4(0.24f, 0.22f, 0.55f, 1.00f); // Dark Blue
colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.13f, 0.40f, 1.00f);
colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_FrameBg];
colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];
colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_Text];
colors[ImGuiCol_Button] = ImVec4(0.44f, 0.69f, 0.83f, 0.40f);
colors[ImGuiCol_ButtonHovered] = ImVec4(0.54f, 0.79f, 0.93f, 1.00f);
colors[ImGuiCol_Header] = colors[ImGuiCol_FrameBg];
colors[ImGuiCol_HeaderHovered] = ImVec4(0.34f, 0.32f, 0.65f, 1.00f);
colors[ImGuiCol_Border] = colors[ImGuiCol_Text];
style.FrameBorderSize = 1.0f;
style.WindowRounding = 0.0f;
style.FrameRounding = 0.0f;
}
static void ApplyVaporwaveDreamTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 0.96f, 1.00f); // #00FFEC
colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.00f, 0.40f, 1.00f); // #2B0066
colors[ImGuiCol_FrameBg] = ImVec4(0.40f, 0.00f, 0.40f, 0.54f); // #660066
colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 0.48f, 1.00f); // #FF007A
colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];
colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.48f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(1.00f, 0.00f, 0.48f, 0.40f);
colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.48f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.48f, 0.31f);
colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.48f, 0.80f);
colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 0.96f, 0.50f);
style.FrameBorderSize = 1.0f;
}
static void ApplyAtomInspiredTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
colors[ImGuiCol_Text] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f); // cccccc
colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f); // 292C2F
colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.23f, 1.00f); // 33363A
colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.54f, 0.65f, 1.00f); // 528AAA
colors[ImGuiCol_CheckMark] = ImVec4(0.32f, 0.54f, 0.65f, 1.00f);
colors[ImGuiCol_SliderGrab] = ImVec4(0.32f, 0.54f, 0.65f, 1.00f);
colors[ImGuiCol_Button] = ImVec4(0.32f, 0.54f, 0.65f, 1.00f);
colors[ImGuiCol_Header] = ImVec4(0.32f, 0.54f, 0.65f, 0.5f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.54f, 0.65f, 0.4f);
}
static void ApplyMidnightOceanTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.00f, 0.59f, 0.69f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.8f, 0.9f, 0.9f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.10f, 0.15f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.15f, 0.20f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.4f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.4f);
style.WindowRounding = 4.0f;
style.FrameRounding = 4.0f;
}
static void ApplyRubyGlowTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 ruby = ImVec4(0.88f, 0.07f, 0.36f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.95f, 0.9f, 0.9f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.05f, 0.05f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.1f, 0.1f, 1.00f);
colors[ImGuiCol_TitleBgActive] = ruby;
colors[ImGuiCol_CheckMark] = ruby;
colors[ImGuiCol_SliderGrab] = ruby;
colors[ImGuiCol_Button] = ruby;
colors[ImGuiCol_Header] = ImVec4(ruby.x, ruby.y, ruby.z, 0.4f);
colors[ImGuiCol_TextSelectedBg] = ImVec4(ruby.x, ruby.y, ruby.z, 0.4f);
style.FrameBorderSize = 1.0f;
colors[ImGuiCol_Border] = ruby;
}
// --- JASNE MOTYWY (NAPRAWIONE I W PEŁNI FUNKCJONALNE) ---
static void ApplyStanfordCardinalLightTheme()
{
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
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
static void ApplyPaperWhiteTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
ImVec4 accent_hover = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
ImVec4 accent_active = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
colors[ImGuiCol_Border] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.85f, 0.85f, 0.70f);
colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.78f, 0.78f, 0.80f);
colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
colors[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_SliderGrabActive] = accent_active;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_ButtonActive] = accent_active;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_HeaderActive] = accent_active;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBg];
colors[ImGuiCol_TabHovered] = colors[ImGuiCol_FrameBgHovered];
colors[ImGuiCol_TabActive] = colors[ImGuiCol_WindowBg];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.36f, 0.36f, 0.36f, 0.35f);

style.FrameRounding = 2.0f;
style.GrabRounding = 2.0f;
style.WindowRounding = 2.0f;
}
static void ApplyCleanSlateBlueTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.23f, 0.53f, 1.00f, 1.00f);
ImVec4 accent_hover = ImVec4(0.33f, 0.63f, 1.00f, 1.00f);
ImVec4 accent_active = ImVec4(0.13f, 0.43f, 0.90f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.06f, 0.16f, 0.26f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.96f, 0.97f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.99f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.99f, 0.98f);
colors[ImGuiCol_Border] = ImVec4(0.80f, 0.85f, 0.90f, 1.00f);
colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.97f, 1.00f, 1.00f);
colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.94f, 1.00f, 1.00f);
colors[ImGuiCol_TitleBg] = ImVec4(0.94f, 0.96f, 0.97f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.92f, 0.94f, 1.00f);
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_SliderGrabActive] = accent_active;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_ButtonActive] = accent_active;
colors[ImGuiCol_Header] = ImVec4(0.85f, 0.90f, 0.95f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_HeaderActive] = accent_active;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBg];
colors[ImGuiCol_TabHovered] = colors[ImGuiCol_FrameBgHovered];
colors[ImGuiCol_TabActive] = colors[ImGuiCol_WindowBg];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.23f, 0.53f, 1.00f, 0.35f);
style.FrameRounding = 4.0f;
style.GrabRounding = 4.0f;
style.WindowRounding = 4.0f;
}
static void ApplyNordicLightTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
ImVec4 accent_hover = ImVec4(0.47f, 0.61f, 0.77f, 1.00f);
ImVec4 accent_active = ImVec4(0.30f, 0.44f, 0.60f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.97f, 0.98f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.96f, 0.97f, 0.98f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.80f, 0.82f, 0.85f, 1.00f);
colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.91f, 0.93f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_ButtonActive] = accent_active;
colors[ImGuiCol_Header] = ImVec4(0.88f, 0.90f, 0.92f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_HeaderActive] = accent;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_SliderGrabActive] = accent_active;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBg];
colors[ImGuiCol_TabHovered] = accent;
colors[ImGuiCol_TabActive] = colors[ImGuiCol_WindowBg];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.37f, 0.51f, 0.67f, 0.35f);
style.FrameRounding = 3.0f;
style.GrabRounding = 3.0f;
style.WindowRounding = 3.0f;
}
static void ApplySaharaCreamTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.83f, 0.21f, 0.51f, 1.00f);
ImVec4 accent_hover = ImVec4(0.93f, 0.31f, 0.61f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.40f, 0.48f, 0.51f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.99f, 0.96f, 0.89f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.93f, 0.85f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.99f, 0.96f, 0.89f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.93f, 0.91f, 0.84f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.88f, 0.86f, 0.80f, 1.00f);
colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.93f, 0.85f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.92f, 0.86f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.83f, 0.21f, 0.51f, 0.35f);
style.FrameRounding = 2.0f;
style.GrabRounding = 2.0f;
}
static void ApplyKyotoSakuraTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.84f, 0.20f, 0.42f, 1.00f);
ImVec4 accent_hover = ImVec4(0.94f, 0.30f, 0.52f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.29f, 0.31f, 0.34f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 0.96f, 0.96f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 0.96f, 0.96f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.96f, 0.90f, 0.90f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.94f, 0.88f, 0.88f, 1.00f);
colors[ImGuiCol_TitleBg] = ImVec4(0.98f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.97f, 0.91f, 0.91f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.84f, 0.20f, 0.42f, 0.35f);
style.WindowRounding = 8.0f;
style.FrameRounding = 8.0f;
style.GrabRounding = 8.0f;
}
static void ApplySpringMintTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.07f, 0.72f, 0.53f, 1.00f);
ImVec4 accent_hover = ImVec4(0.17f, 0.82f, 0.63f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.20f, 0.29f, 0.37f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.98f, 0.97f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.96f, 0.94f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.95f, 0.98f, 0.97f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.94f, 0.91f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.80f, 0.92f, 0.88f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.88f, 0.95f, 0.92f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.07f, 0.72f, 0.53f, 0.35f);
style.FrameRounding = 5.0f;
style.GrabRounding = 5.0f;
}
static void ApplyAquaMarineTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.09f, 0.64f, 0.72f, 1.00f);
ImVec4 accent_hover = ImVec4(0.19f, 0.74f, 0.82f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.00f, 0.20f, 0.40f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 1.00f, 1.00f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.98f, 0.98f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 1.00f, 1.00f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.98f, 0.98f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.80f, 0.95f, 0.95f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.88f, 0.99f, 0.99f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.09f, 0.64f, 0.72f, 0.35f);
style.WindowRounding = 0.0f;
style.FrameRounding = 0.0f;
}
static void ApplyLavenderHazeTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.60f, 0.46f, 0.98f, 1.00f);
ImVec4 accent_hover = ImVec4(0.70f, 0.56f, 1.00f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.29f, 0.31f, 0.34f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 1.00f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.95f, 0.94f, 0.99f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 1.00f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.90f, 0.98f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.88f, 0.86f, 0.96f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.92f, 0.99f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.60f, 0.46f, 0.98f, 0.35f);
style.FrameRounding = 20.0f;
style.GrabRounding = 20.0f;
}
static void ApplyPeachSorbetTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(1.00f, 0.53f, 0.53f, 1.00f);
ImVec4 accent_hover = ImVec4(1.00f, 0.63f, 0.63f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.47f, 0.33f, 0.28f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 0.97f, 0.94f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.99f, 0.95f, 0.92f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 0.97f, 0.94f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.94f, 0.88f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.98f, 0.90f, 0.85f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(1.00f, 0.95f, 0.90f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.53f, 0.53f, 0.35f);
style.WindowRounding = 6.0f;
style.FrameRounding = 6.0f;
}
static void ApplySunnyCitrusTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.98f, 0.69f, 0.02f, 1.00f);
ImVec4 accent_hover = ImVec4(1.00f, 0.79f, 0.12f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.53f, 0.27f, 0.04f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 0.98f, 0.90f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 0.97f, 0.87f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 0.98f, 0.90f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.96f, 0.82f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.99f, 0.92f, 0.75f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(1.00f, 0.97f, 0.85f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.69f, 0.02f, 0.35f);
style.WindowRounding = 0.0f;
style.FrameRounding = 4.0f;
}
static void ApplyHarvardCrimsonTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.65f, 0.11f, 0.19f, 1.00f);
ImVec4 accent_hover = ImVec4(0.75f, 0.21f, 0.29f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.65f, 0.11f, 0.19f, 0.35f);
style.FrameRounding = 0.0f;
style.WindowRounding = 0.0f;
}
static void ApplyOxfordBlueTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.00f, 0.13f, 0.28f, 1.00f);
ImVec4 accent_hover = ImVec4(0.05f, 0.23f, 0.38f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.20f, 0.25f, 0.33f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.97f, 0.98f, 0.99f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.98f, 0.99f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.93f, 0.95f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.88f, 0.89f, 0.92f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.95f, 0.97f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.13f, 0.28f, 0.35f);
}
static void ApplySilverlightTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
ImVec4 accent_hover = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.46f, 0.46f, 0.46f, 0.35f);
style.FrameRounding = 0.0f;
style.WindowRounding = 0.0f;
style.GrabRounding = 0.0f;
}
static void ApplyAlabasterShellTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(0.69f, 0.61f, 0.85f, 1.00f);
ImVec4 accent_hover = ImVec4(0.79f, 0.71f, 0.95f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.36f, 0.35f, 0.35f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.96f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.95f, 0.94f, 0.92f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.96f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.91f, 0.89f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.88f, 0.87f, 0.85f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.94f, 0.93f, 0.91f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(0.69f, 0.61f, 0.85f, 0.35f);
}
static void ApplyCoralReefTheme() {
ImGuiStyle& style = ImGui::GetStyle();
auto& colors = style.Colors;
ImVec4 accent = ImVec4(1.00f, 0.50f, 0.31f, 1.00f);
ImVec4 accent_hover = ImVec4(1.00f, 0.60f, 0.41f, 1.00f);
colors[ImGuiCol_Text] = ImVec4(0.00f, 0.30f, 0.25f, 1.00f);
colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.99f, 0.98f, 1.00f);
colors[ImGuiCol_ChildBg] = ImVec4(0.92f, 0.98f, 0.97f, 1.00f);
colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.99f, 0.98f, 0.98f);
colors[ImGuiCol_FrameBg] = ImVec4(0.88f, 0.98f, 0.95f, 1.00f);
colors[ImGuiCol_Border] = ImVec4(0.84f, 0.96f, 0.93f, 1.00f);
colors[ImGuiCol_TitleBgActive] = accent;
colors[ImGuiCol_Button] = accent;
colors[ImGuiCol_ButtonHovered] = accent_hover;
colors[ImGuiCol_Header] = ImVec4(0.90f, 0.99f, 0.96f, 1.00f);
colors[ImGuiCol_HeaderHovered] = accent_hover;
colors[ImGuiCol_CheckMark] = accent;
colors[ImGuiCol_SliderGrab] = accent;
colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.50f, 0.31f, 0.35f);
style.FrameRounding = 3.0f;
style.GrabRounding = 3.0f;
}
} // namespace ThemeManager
