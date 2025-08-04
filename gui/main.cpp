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
// Dołączamy nagłówki do zarządzania motywami i konfiguracją
#include "Themes.h"
#include "ConfigManager.h"

// --- Zmienne Globalne Stanu Aplikacji ---
static AppConfig app_config; // Globalna struktura przechowująca stan aplikacji
static char role_buffer[1024] = "Jesteś ekspertem programistą C++ z 20-letnim doświadczeniem w systemach o niskim opóźnieniu i wysokiej wydajności.";
static char task_buffer[2048] = "Przeanalizuj poniższy kod. Zidentyfikuj potencjalne błędy, 'code smells', oraz zaproponuj refaktoryzację w celu poprawy czytelności i wydajności.";
static char context_buffer[65536] = ""; // Duży bufor na kod
static char constraints_buffer[1024] = "- Nie używaj bibliotek zewnętrznych poza C++17 STL.\n- Zachowaj istniejącą funkcjonalność.\n- Odpowiadaj w języku polskim.";
static char output_format_buffer[1024] = "Zaproponuj zmiany w formacie 'diff'. Każdą sugestię opatrz zwięzłym uzasadnieniem.";
static std::string final_prompt;

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

// NOWA, POPRAWIONA implementacja splittera, kompatybilna ze starszymi wersjami ImGui.
static void VSplitter(const char* str_id, float* width, float min_width, float max_width)
{
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    
    // Używamy standardowego przycisku jako obszaru interakcji dla splittera.
    // Jego wysokość (-1) sprawia, że wypełnia on całą dostępną przestrzeń pionową.
    ImGui::Button(str_id, ImVec2(8.0f, -1)); 
    
    ImGui::PopStyleColor(3);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    
    if (ImGui::IsItemActive())
    {
        *width += ImGui::GetIO().MouseDelta.x;
    }

    // Ograniczenia szerokości
    if (*width < min_width) *width = min_width;
    if (*width > max_width) *width = max_width;

    ImGui::SameLine();
}


void RenderPromptConstructor()
{
    // Całkowita dostępna szerokość wewnątrz zakładki
    float available_width = ImGui::GetContentRegionAvail().x;
    float splitter_width = 8.0f;
    float min_pane_width = 200.0f;

    // Obliczenie maksymalnej dopuszczalnej szerokości lewego panelu
    float max_left_pane_width = available_width - splitter_width - min_pane_width;
    if (app_config.left_pane_width > max_left_pane_width) {
        app_config.left_pane_width = max_left_pane_width;
    }
     if (app_config.left_pane_width < min_pane_width) {
        app_config.left_pane_width = min_pane_width;
    }

    // Lewa kolumna (Komponenty Promptu)
    // Podajemy JAWNĄ szerokość i wysokość -1 (wypełnij)
    ImGui::BeginChild("LeftPane", ImVec2(app_config.left_pane_width, -1), ImGuiChildFlags_Border);
        if (ImGui::CollapsingHeader("Rola / Persona", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputTextMultiline("##Role", role_buffer, sizeof(role_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 5));
        }
        if (ImGui::CollapsingHeader("Zadanie", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputTextMultiline("##Task", task_buffer, sizeof(task_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 8));
        }
        if (ImGui::CollapsingHeader("Ograniczenia", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputTextMultiline("##Constraints", constraints_buffer, sizeof(constraints_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 5));
        }
        if (ImGui::CollapsingHeader("Format Wyjściowy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputTextMultiline("##OutputFormat", output_format_buffer, sizeof(output_format_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 5));
        }
    ImGui::EndChild();

    // Splitter - renderowany pomiędzy dwoma panelami
    VSplitter("##VSplitter", &app_config.left_pane_width, min_pane_width, max_left_pane_width);

    // Prawa kolumna (Kontekst i Wynik)
    // Szerokość 0 oznacza "wypełnij pozostałą przestrzeń", wysokość -1 "wypełnij w pionie"
    ImGui::BeginChild("RightPane", ImVec2(0, -1), ImGuiChildFlags_None); 
        if(ImGui::BeginTabBar("RightPaneTabs"))
        {
            if (ImGui::BeginTabItem("Kontekst"))
            {
                if (ImGui::Button("Wczytaj kontekst z bieżącego katalogu (dirpacker)", ImVec2(-FLT_MIN, 0)))
                {
                    try {
                        std::stringstream packed_stream;
                        dp::PackerConfig config = {".", packed_stream};
                        dp::pack(config);
                        strncpy(context_buffer, packed_stream.str().c_str(), sizeof(context_buffer) - 1);
                        context_buffer[sizeof(context_buffer) - 1] = '\0';
                    } catch (const std::exception& e) {
                        std::string error_msg = "Błąd dirpacker: ";
                        error_msg += e.what();
                        strncpy(context_buffer, error_msg.c_str(), sizeof(context_buffer) - 1);
                        context_buffer[sizeof(context_buffer) - 1] = '\0';
                    }
                }
                ImGui::InputTextMultiline("##Context", context_buffer, sizeof(context_buffer), ImVec2(-FLT_MIN, -FLT_MIN));
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Finalny Prompt"))
            {
                std::stringstream ss;
                ss << "### Rola:\n" << role_buffer << "\n\n";
                ss << "### Zadanie:\n" << task_buffer << "\n\n";
                if (strlen(constraints_buffer) > 0) ss << "### Ograniczenia:\n" << constraints_buffer << "\n\n";
                if (strlen(output_format_buffer) > 0) ss << "### Format Wyjściowy:\n" << output_format_buffer << "\n\n";
                ss << "### Kontekst (Kod do analizy):\n```\n" << context_buffer << "\n```";
                final_prompt = ss.str();

                int token_count = EstimateTokens(final_prompt.c_str());
                ImGui::Text("Szacunkowa liczba tokenów: %d", token_count);
                ImGui::SameLine();
                if (ImGui::Button("Kopiuj do schowka"))
                {
                    ImGui::SetClipboardText(final_prompt.c_str());
                }
                
                ImGui::Separator();
                ImGui::BeginChild("FinalPromptScrollRegion", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::TextWrapped("%s", final_prompt.c_str());
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    ImGui::EndChild();
}


// --- Główna Funkcja Renderująca UI ---
void RenderUI()
{
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Główny Kontener", nullptr, window_flags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Motywy"))
        {
            for (int i = 0; i < ThemeManager::theme_count; ++i)
            {
                if (ImGui::MenuItem(ThemeManager::theme_names[i], NULL, ThemeManager::current_theme == (ThemeManager::AppTheme)i))
                {
                    ThemeManager::ApplyTheme((ThemeManager::AppTheme)i);
                    app_config.theme = ThemeManager::current_theme; // Zapisz zmianę w konfiguracji
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    if (ImGui::BeginTabBar("MainTabBar"))
    {
        if (ImGui::BeginTabItem("Konstruktor Promptu"))
        {
            RenderPromptConstructor();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Zarządzanie Szablonami"))
        {
            ImGui::Text("Ta sekcja jest w budowie!");
            ImGui::Text("W przyszłości znajdą się tutaj narzędzia do zapisywania, wczytywania i zarządzania szablonami promptów oraz gotowymi wycinkami (snippets).");
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Ustawienia"))
        {
             ImGui::Text("Miejsce na przyszłe ustawienia aplikacji.");
             ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}


int main(int, char **)
{
    // Wczytaj konfigurację na samym początku
    ConfigManager::LoadConfig(app_config);

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
    SDL_Window *window = SDL_CreateWindow("AI Prompter (Prof. Marcin & Prof. Gemini)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, window_flags);
    if (window == nullptr) {
        ThrowSDLError("Błąd podczas tworzenia okna SDL.");
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0); // Wyłącz V-Sync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Ustaw motyw wczytany z pliku
    ThemeManager::ApplyTheme(app_config.theme);

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
    const int IDLE_FPS = 15;
    const Uint32 IDLE_TIMEOUT_MS = 200;
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
                if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
                    done = true;
            }
        }
        
        if (event_occured)
        {
            last_event_time = SDL_GetTicks();
            is_idle = false;
            if (event.type != 0) {
                 ImGui_ImplSDL2_ProcessEvent(&event);
                 if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)))
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
        
        ImVec4 clear_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        Uint32 time_elapsed = SDL_GetTicks() - frame_start;
        if (time_elapsed < frame_duration)
        {
            SDL_Delay(frame_duration - time_elapsed);
        }
    }
    
    // Zapisz konfigurację przed zamknięciem
    ConfigManager::SaveConfig(app_config);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}