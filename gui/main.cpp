#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <memory> 

#include <SDL.h>
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "dirpacker/dirpacker.h"
#include "Themes.h"
#include "AppState.h"
#include "ConfigManager.h"
#include "DatabaseManager.h"
#include "I18nManager.h"

// --- Zmienne Globalne Stanu Aplikacji ---
static AppState app_state;
static std::unique_ptr<DatabaseManager> db_manager;
static std::vector<std::string> available_theme_names;

static std::string current_role;
static std::string current_task;
static std::string current_constraints;
static std::string current_output_format;

static std::string current_prompt_lang_code = "pl_PL";
static std::string current_prompt_template_key = "refactor_prompt_name";

static std::string context_buffer;
static std::string playground_output_buffer;
static std::string final_prompt;

// --- Funkcje Pomocnicze ---
int EstimateTokens(const char *text) { return text ? static_cast<int>(strlen(text) / 4) : 0; }
void ThrowSDLError(const std::string &message) { throw std::runtime_error(message + " SDL_Error: " + SDL_GetError()); }

void LoadPromptTemplate(const std::string& name_key, const std::string& lang_code) {
    PromptContent content = I18nManager::GetInstance().GetPromptContent(name_key, lang_code);
    current_role = content.role;
    current_task = content.task;
    current_constraints = content.constraints;
    current_output_format = content.output_format;
}

static void VSplitter(const char* str_id, float* width) {
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::Button(str_id, ImVec2(8.0f, -1));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    if (ImGui::IsItemActive()) *width += ImGui::GetIO().MouseDelta.x;
    ImGui::SameLine();
}

// --- Funkcje Renderujące ---

void RenderLibraryPane(float width) {
    ImGui::BeginChild("LibraryPane", ImVec2(width, 0), ImGuiChildFlags_Border);
    ImGui::Text("%s", L("library_pane_title"));
    ImGui::Separator();
    if (ImGui::TreeNodeEx(L("prompt_templates_header"), ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Selectable(L("refactor_prompt_name"), current_prompt_template_key == "refactor_prompt_name")) {
            current_prompt_template_key = "refactor_prompt_name";
            LoadPromptTemplate(current_prompt_template_key, current_prompt_lang_code);
        }
        ImGui::TreePop();
    }
    ImGui::EndChild();
}

void RenderComposerPane() {
    ImGui::BeginChild("ComposerPaneContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2.2f), false);
    if (ImGui::BeginTabBar("ComposerTabs")) {
        if (ImGui::BeginTabItem(L(current_prompt_template_key.c_str()))) {
            ImGui::BeginChild("ComposerLeft", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4.0f, 0), ImGuiChildFlags_None);
                if (ImGui::CollapsingHeader(L("role_header"), ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Role",&current_role, ImVec2(-1, ImGui::GetTextLineHeight() * 4));
                if (ImGui::CollapsingHeader(L("task_header"), ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Task",&current_task, ImVec2(-1, ImGui::GetTextLineHeight() * 7));
                if (ImGui::CollapsingHeader(L("constraints_header"), ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Constraints",&current_constraints, ImVec2(-1, ImGui::GetTextLineHeight() * 4));
                if (ImGui::CollapsingHeader(L("output_format_header"), ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##OutputFormat",&current_output_format, ImVec2(-1, ImGui::GetTextLineHeight() * 4));
            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginChild("ComposerRight", ImVec2(0, 0), ImGuiChildFlags_None);
                if (ImGui::Button(L("load_code_button"), ImVec2(-1, 0))) { /* Logika Dirpacker */ }
                ImGui::InputTextMultiline("##Context", &context_buffer, ImVec2(-1, -1));
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();

    ImGui::Separator();
    ImGui::TextUnformatted(L("prompt_language_label"));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    const auto& prompt_langs = I18nManager::GetInstance().GetAvailablePromptLanguages();
    if (prompt_langs.count(current_prompt_lang_code)) {
        if (ImGui::BeginCombo("##PromptLang", prompt_langs.at(current_prompt_lang_code).c_str())) {
            for (const auto& [code, name] : prompt_langs) {
                const bool is_selected = (current_prompt_lang_code == code);
                if (ImGui::Selectable(name.c_str(), is_selected)) {
                    current_prompt_lang_code = code;
                    LoadPromptTemplate(current_prompt_template_key, current_prompt_lang_code);
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}

void RenderPlaygroundPane(float width) {
    ImGui::BeginChild("PlaygroundPane", ImVec2(width, 0), ImGuiChildFlags_Border);
    ImGui::Text("%s", L("playground_pane_title"));
    ImGui::Separator();
    
    std::stringstream ss;
    ss << current_role << "\n\n" << current_task << "\n\n" << current_constraints << "\n\n" << current_output_format << "\n\n---\nKOD:\n" << context_buffer;
    final_prompt = ss.str();

    if (ImGui::Button(L("copy_prompt_button"), ImVec2(-1, 0))) {
        ImGui::SetClipboardText(final_prompt.c_str());
    }

    if (ImGui::BeginTabBar("OutputTabs")) {
        if (ImGui::BeginTabItem(L("model_response_tab"))) {
            ImGui::InputTextMultiline("##PlaygroundOutput", &playground_output_buffer, ImVec2(-1, -1));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(L("final_prompt_tab"))) {
             ImGui::BeginChild("FinalPromptScroll", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
             ImGui::TextUnformatted(final_prompt.c_str());
             ImGui::EndChild();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void RenderStatusBar() {
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 5));
    
    char theme_buffer[256];
    snprintf(theme_buffer, sizeof(theme_buffer), L("status_bar_theme"), app_state.theme_name.c_str());
    float theme_text_width = ImGui::CalcTextSize(theme_buffer).x;

    int token_count = EstimateTokens(final_prompt.c_str());
    ImGui::Text(L("status_bar_tokens"), token_count);
    
    ImGui::SameLine(ImGui::GetWindowWidth() - theme_text_width - ImGui::GetStyle().ItemSpacing.x);
    ImGui::TextUnformatted(theme_buffer);
    
    ImGui::PopStyleVar();
}

void RenderUI() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Główny Kontener", nullptr, window_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(L("themes_menu"))) {
            for (const auto& theme_name : available_theme_names) {
                if (ImGui::MenuItem(theme_name.c_str(), NULL, app_state.theme_name == theme_name)) {
                    ThemeManager::ApplyTheme(theme_name, db_manager->getDB());
                    app_state.theme_name = theme_name;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(L("ui_language_menu"))) {
            const auto& ui_langs = I18nManager::GetInstance().GetAvailableUILanguages();
            for (const auto& [code, name] : ui_langs) {
                if (ImGui::MenuItem(name.c_str(), NULL, I18nManager::GetInstance().GetCurrentUILanguage() == code)) {
                    I18nManager::GetInstance().SetUILanguage(code);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    float status_bar_height = ImGui::GetFrameHeightWithSpacing() + 10;
    float available_height = ImGui::GetContentRegionAvail().y - status_bar_height;

    ImGui::BeginChild("PanelsContainer", ImVec2(0, available_height), ImGuiChildFlags_None);
    {
        float total_width = ImGui::GetContentRegionAvail().x;
        float min_pane_width = 200.0f;
        float splitter_width = 8.0f;

        if (app_state.library_pane_width < min_pane_width) app_state.library_pane_width = min_pane_width;
        if (app_state.playground_pane_width < min_pane_width) app_state.playground_pane_width = min_pane_width;
        if (app_state.library_pane_width + app_state.playground_pane_width + (2 * splitter_width) > total_width) {
            app_state.library_pane_width = total_width * 0.25f;
            app_state.playground_pane_width = total_width * 0.30f;
        }
        
        RenderLibraryPane(app_state.library_pane_width);
        VSplitter("##VSplitter1", &app_state.library_pane_width);
        
        float composer_width = total_width - app_state.library_pane_width - app_state.playground_pane_width - (2 * splitter_width);
        ImGui::BeginChild("ComposerPane", ImVec2(composer_width, 0), ImGuiChildFlags_None);
        RenderComposerPane();
        ImGui::EndChild();
        
        VSplitter("##VSplitter2", &composer_width);
        app_state.playground_pane_width = total_width - app_state.library_pane_width - composer_width - (2 * splitter_width);
        RenderPlaygroundPane(app_state.playground_pane_width);
    }
    ImGui::EndChild();
    
    RenderStatusBar();

    ImGui::End();
}

int main(int, char **) {
    ConfigManager::LoadConfig(app_state);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) ThrowSDLError("Błąd podczas inicjalizacji SDL.");

    try {
        fs::path config_dir = ConfigManager::GetConfigDirPath();
        db_manager = std::make_unique<DatabaseManager>(config_dir / "ai_prompter.sqlite");
        
        I18nManager::GetInstance().Initialize(db_manager->getDB());
        ThemeManager::InitializeAndSeedThemes(db_manager->getDB());
        
        available_theme_names = ThemeManager::GetThemeNames(db_manager->getDB());

        // TODO: Wczytaj zapisane języki z pliku .conf lub z bazy danych
        I18nManager::GetInstance().SetUILanguage("pl_PL"); 
        current_prompt_lang_code = "pl_PL";
        
        LoadPromptTemplate(current_prompt_template_key, current_prompt_lang_code);

    } catch (const std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Błąd Krytyczny Bazy Danych", e.what(), NULL);
        return 1;
    }

    const char* glsl_version = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("AI Prompter v2 (Prof. Marcin & Prof. Gemini)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 1000, window_flags);
    if (window == nullptr) ThrowSDLError("Błąd podczas tworzenia okna SDL.");
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ThemeManager::ApplyTheme(app_state.theme_name, db_manager->getDB());

    char* base_path_ptr = SDL_GetBasePath();
    std::string font_path = std::string(base_path_ptr ? base_path_ptr : "./") + "assets/fonts/Roboto-Regular.ttf";
    SDL_free(base_path_ptr);
    
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const int IDLE_FPS = 15;
    const Uint32 IDLE_TIMEOUT_MS = 200;
    Uint32 last_event_time = SDL_GetTicks();
    bool is_idle = false, done = false;

    while (!done) {
        SDL_Event event;
        bool event_occured = is_idle ? SDL_WaitEventTimeout(&event, 1000 / IDLE_FPS) : SDL_PollEvent(&event);
        
        while (event_occured) {
            is_idle = false;
            last_event_time = SDL_GetTicks();
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
                done = true;
            if (!is_idle && !SDL_PollEvent(&event)) event_occured = false;
        }
        
        if (!is_idle && (SDL_GetTicks() - last_event_time > IDLE_TIMEOUT_MS)) is_idle = true;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        RenderUI();
        ImGui::Render();

        ImVec4 clear_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ConfigManager::SaveConfig(app_state);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}