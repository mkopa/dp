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

#include "dirpacker/dirpacker.h"
#include "Themes.h"
#include "AppState.h"
#include "ConfigManager.h"
#include "DatabaseManager.h"

// --- Zmienne Globalne Stanu Aplikacji ---
static AppState app_state;
static std::unique_ptr<DatabaseManager> db_manager;
static std::vector<std::string> available_theme_names;
static char role_buffer[1024] = "Jesteś ekspertem programistą C++ z 20-letnim doświadczeniem w systemach o niskim opóźnieniu i wysokiej wydajności.";
static char task_buffer[2048] = "Przeanalizuj poniższy kod. Zidentyfikuj potencjalne błędy, 'code smells', oraz zaproponuj refaktoryzację w celu poprawy czytelności i wydajności.";
static char context_buffer[65536] = "";
static char constraints_buffer[1024] = "- Nie używaj bibliotek zewnętrznych poza C++17 STL.\n- Zachowaj istniejącą funkcjonalność.\n- Odpowiadaj w języku polskim.";
static char output_format_buffer[1024] = "Zaproponuj zmiany w formacie 'diff'. Każdą sugestię opatrz zwięzłym uzasadnieniem.";
static char playground_output_buffer[65536] = "Tutaj wklej odpowiedź z modelu AI, aby ją przeanalizować i porównać...";
static std::string final_prompt;

// --- Funkcje Pomocnicze ---
int EstimateTokens(const char *text) { return text ? static_cast<int>(strlen(text) / 4) : 0; }
void ThrowSDLError(const std::string &message) { throw std::runtime_error(message + " SDL_Error: " + SDL_GetError()); }

static void VSplitter(const char* str_id, float* width)
{
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

// --- Funkcje Renderujące Poszczególne Panele ---

void RenderLibraryPane(float width) {
    ImGui::BeginChild("LibraryPane", ImVec2(width, 0), ImGuiChildFlags_Border);
    ImGui::Text("Biblioteka Zasobów");
    ImGui::Separator();
    if (ImGui::TreeNodeEx("Szablony Promptów", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Selectable("Refaktoryzacja C++");
        ImGui::Selectable("Generowanie Dokumentacji");
        ImGui::Selectable("Analiza Błędów Logiki");
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Wycinki (Snippets)", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Selectable("[Rola] Senior C++ Dev");
        ImGui::Selectable("[Ograniczenie] Tylko STL");
        ImGui::Selectable("[Format] Markdown z diff");
        ImGui::TreePop();
    }
    ImGui::EndChild();
}

void RenderComposerPane() {
    if (ImGui::BeginTabBar("ComposerTabs")) {
        if (ImGui::BeginTabItem("Prompt: Refaktoryzacja Silnika Fizyki v1")) {
            ImGui::BeginChild("ComposerLeft", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0), ImGuiChildFlags_None);
                if (ImGui::CollapsingHeader("Rola / Persona", ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Role", role_buffer, sizeof(role_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 4));
                if (ImGui::CollapsingHeader("Zadanie", ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Task", task_buffer, sizeof(task_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 7));
                if (ImGui::CollapsingHeader("Ograniczenia", ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##Constraints", constraints_buffer, sizeof(constraints_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 4));
                if (ImGui::CollapsingHeader("Format Wyjściowy", ImGuiTreeNodeFlags_DefaultOpen))
                    ImGui::InputTextMultiline("##OutputFormat", output_format_buffer, sizeof(output_format_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 4));
            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginChild("ComposerRight", ImVec2(0, 0), ImGuiChildFlags_None);
                if (ImGui::Button("Wczytaj kod z katalogu", ImVec2(-1, 0))) { /* Logika Dirpacker */ }
                ImGui::InputTextMultiline("##Context", context_buffer, sizeof(context_buffer), ImVec2(-1, -1));
            ImGui::EndChild();
            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("+")) { /* Logika dodawania nowej zakładki */ }
        ImGui::EndTabBar();
    }
}

void RenderPlaygroundPane(float width) {
    ImGui::BeginChild("PlaygroundPane", ImVec2(width, 0), ImGuiChildFlags_Border);
    ImGui::Text("Plac Zabaw AI");
    ImGui::Separator();
    
    std::stringstream ss;
    ss << role_buffer << "\n\n" << task_buffer << "\n\n" << constraints_buffer << "\n\n" << output_format_buffer << "\n\n---\nKOD:\n" << context_buffer;
    final_prompt = ss.str();

    if (ImGui::Button("Kopiuj gotowy prompt", ImVec2(-1, 0))) {
        ImGui::SetClipboardText(final_prompt.c_str());
    }

    if (ImGui::BeginTabBar("OutputTabs")) {
        if (ImGui::BeginTabItem("Odpowiedź Modelu")) {
            ImGui::InputTextMultiline("##PlaygroundOutput", playground_output_buffer, sizeof(playground_output_buffer), ImVec2(-1, -1));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Finalny prompt (podgląd)")) {
             ImGui::BeginChild("FinalPromptScroll", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
             ImGui::TextWrapped("%s", final_prompt.c_str());
             ImGui::EndChild();
             ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void RenderStatusBar() {
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 5));
    
    float theme_text_width = ImGui::CalcTextSize(app_state.theme_name.c_str()).x + ImGui::CalcTextSize("Motyw: ").x;
    int token_count = EstimateTokens(final_prompt.c_str());
    ImGui::Text("Szacowana liczba tokenów: %d", token_count);
    
    ImGui::SameLine(ImGui::GetWindowWidth() - theme_text_width - ImGui::GetStyle().ItemSpacing.x);
    ImGui::Text("Motyw: %s", app_state.theme_name.c_str());
    
    ImGui::PopStyleVar();
}

void RenderUI() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Główny Kontener", nullptr, window_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Motywy")) {
            for (const auto& theme_name : available_theme_names) {
                if (ImGui::MenuItem(theme_name.c_str(), NULL, app_state.theme_name == theme_name)) {
                    ThemeManager::ApplyTheme(theme_name, db_manager->getDB());
                    app_state.theme_name = theme_name;
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
        ThemeManager::InitializeAndSeedThemes(db_manager->getDB());
        available_theme_names = ThemeManager::GetThemeNames(db_manager->getDB());
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