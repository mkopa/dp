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

// Prosta funkcja do szacowania liczby tokenów
int EstimateTokens(const char* text) {
    if (!text) return 0;
    return static_cast<int>(strlen(text) / 4);
}

// Funkcja pomocnicza do obsługi błędów
void ThrowSDLError(const std::string& message) {
    std::string error_message = message + " SDL_Error: " + SDL_GetError();
    throw std::runtime_error(error_message);
}

// --- Funkcja Renderująca UI ---
void RenderUI() {
    // Główne, niewidoczne okno, które służy jako kontener dla layoutu
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("Główny Kontener", nullptr, window_flags);

    // Layout oparty na tabeli z przesuwalnymi granicami
    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("GłównyLayout", 3, table_flags)) {
        ImGui::TableSetupColumn("Konstruktor", ImGuiTableColumnFlags_WidthStretch, 0.30f);
        ImGui::TableSetupColumn("Kontekst", ImGuiTableColumnFlags_WidthStretch, 0.35f);
        ImGui::TableSetupColumn("Wynik", ImGuiTableColumnFlags_WidthStretch, 0.35f);
        
        // --- Panel 1: Konstruktor Promptu ---
        ImGui::TableNextColumn();
        ImGui::Text("Konstruktor Promptu");
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Rola / Persona", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##Role", role_buffer, sizeof(role_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
        }
        if (ImGui::CollapsingHeader("Zadanie", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##Task", task_buffer, sizeof(task_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 6));
        }
        if (ImGui::CollapsingHeader("Ograniczenia", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##Constraints", constraints_buffer, sizeof(constraints_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
        }
        if (ImGui::CollapsingHeader("Format Wyjściowy", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##OutputFormat", output_format_buffer, sizeof(output_format_buffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
        }

        // --- Panel 2: Zarządzanie Kontekstem ---
        ImGui::TableNextColumn();
        ImGui::Text("Kontekst (Kod Źródłowy)");
        ImGui::Separator();
        if (ImGui::Button("Wczytaj kontekst z katalogu (dirpacker)", ImVec2(-FLT_MIN, 0))) {
            // TODO: Zaimplementować okno dialogowe wyboru katalogu
            strcpy(context_buffer, "--- MIEJSCE NA WYNIK DZIAŁANIA DIRPACKER ---\n");
        }
        // <<< KLUCZOWA ZMIANA TUTAJ
        ImGui::InputTextMultiline("##Context", context_buffer, sizeof(context_buffer), ImVec2(-FLT_MIN, -FLT_MIN));

        // --- Panel 3: Finalny Prompt i Akcje ---
        ImGui::TableNextColumn();
        ImGui::Text("Finalny Prompt");
        ImGui::Separator();

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
        if (ImGui::Button("Kopiuj", ImVec2(-FLT_MIN, 0))) {
            ImGui::SetClipboardText(final_prompt.c_str());
        }
        // <<< KLUCZOWA ZMIANA TUTAJ
        ImGui::InputTextMultiline("##FinalPrompt", (char*)final_prompt.c_str(), final_prompt.length() + 1, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_ReadOnly);

        ImGui::EndTable();
    }
    ImGui::End();
}


int main(int, char**) {
    // ... (cała funkcja main pozostaje bez zmian)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { ThrowSDLError("Błąd podczas inicjalizacji SDL."); }
    const char* glsl_version = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("AI Prompter (Prof. Marcin & Prof. Gemini)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    const int ACTIVE_FPS = 60;
    const int IDLE_FPS = 4;
    const Uint32 IDLE_TIMEOUT_MS = 500;
    Uint32 last_event_time = SDL_GetTicks();
    bool is_idle = false;

    bool done = false;
    while (!done) {
        SDL_Event event;
        bool event_occured = false;
        if (is_idle) { if (SDL_WaitEventTimeout(&event, 1000 / IDLE_FPS)) event_occured = true;
        } else { while (SDL_PollEvent(&event)) { event_occured = true; ImGui_ImplSDL2_ProcessEvent(&event); if (event.type == SDL_QUIT) done = true; if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) done = true; } }
        if (event_occured) { last_event_time = SDL_GetTicks(); if (is_idle) is_idle = false; if (event.type != 0) { ImGui_ImplSDL2_ProcessEvent(&event); if (event.type == SDL_QUIT) done = true; if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) done = true; }
        } else { if (!is_idle && (SDL_GetTicks() - last_event_time > IDLE_TIMEOUT_MS)) is_idle = true; }
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
        if (time_elapsed < frame_duration) {
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