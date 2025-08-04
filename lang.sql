-- Krok 1: Dodaj języki
INSERT INTO languages (lang_code, name_native) VALUES ('en_US', 'English'), ('pl_PL', 'Polski');

-- Krok 2: Dodaj tłumaczenia UI
INSERT INTO ui_translations (key, lang_code, value) VALUES
    ('themes_menu', 'en_US', 'Themes'),
    ('themes_menu', 'pl_PL', 'Motywy'),
    ('library_pane_title', 'en_US', 'Resource Library'),
    ('library_pane_title', 'pl_PL', 'Biblioteka Zasobów'),
    ('prompt_templates_header', 'en_US', 'Prompt Templates'),
    ('prompt_templates_header', 'pl_PL', 'Szablony Promptów'),
    ('snippets_header', 'en_US', 'Snippets'),
    ('snippets_header', 'pl_PL', 'Wycinki (Snippets)'),
    ('composer_pane_title', 'en_US', 'Composer'),
    ('composer_pane_title', 'pl_PL', 'Kompozytor'),
    ('role_header', 'en_US', 'Role / Persona'),
    ('role_header', 'pl_PL', 'Rola / Persona'),
    ('task_header', 'en_US', 'Task'),
    ('task_header', 'pl_PL', 'Zadanie'),
    ('constraints_header', 'en_US', 'Constraints'),
    ('constraints_header', 'pl_PL', 'Ograniczenia'),
    ('output_format_header', 'en_US', 'Output Format'),
    ('output_format_header', 'pl_PL', 'Format Wyjściowy'),
    ('load_code_button', 'en_US', 'Load code from directory'),
    ('load_code_button', 'pl_PL', 'Wczytaj kod z katalogu'),
    ('playground_pane_title', 'en_US', 'AI Playground'),
    ('playground_pane_title', 'pl_PL', 'Plac Zabaw AI'),
    ('copy_prompt_button', 'en_US', 'Copy final prompt'),
    ('copy_prompt_button', 'pl_PL', 'Kopiuj gotowy prompt'),
    ('model_response_tab', 'en_US', 'Model Response'),
    ('model_response_tab', 'pl_PL', 'Odpowiedź Modelu'),
    ('final_prompt_tab', 'en_US', 'Final Prompt (Preview)'),
    ('final_prompt_tab', 'pl_PL', 'Finalny prompt (podgląd)'),
    ('status_bar_tokens', 'en_US', 'Estimated tokens: %d'),
    ('status_bar_tokens', 'pl_PL', 'Szacowana liczba tokenów: %d'),
    ('status_bar_theme', 'en_US', 'Theme: %s'),
    ('status_bar_theme', 'pl_PL', 'Motyw: %s'),
    ('refactor_prompt_name', 'en_US', 'C++ Refactoring'),
    ('refactor_prompt_name', 'pl_PL', 'Refaktoryzacja C++');

-- Krok 3: Dodaj szablon promptu (klucz nazwy)
INSERT INTO prompt_templates (name_key) VALUES ('refactor_prompt_name');

-- Krok 4: Dodaj tłumaczenia dla szablonu promptu
INSERT INTO prompt_translations (template_id, lang_code, role, task, constraints, output_format) VALUES
    (1, 'en_US', 'You are an expert C++ programmer with 20 years of experience in low-latency, high-performance systems.', 'Analyze the code below. Identify potential bugs, code smells, and propose refactoring to improve readability and performance.', '- Do not use external libraries beyond the C++17 STL.\n- Preserve existing functionality.', 'Propose changes in a "diff" format. Provide a concise justification for each suggestion.'),
    (1, 'pl_PL', 'Jesteś ekspertem programistą C++ z 20-letnim doświadczeniem w systemach o niskim opóźnieniu i wysokiej wydajności.', 'Przeanalizuj poniższy kod. Zidentyfikuj potencjalne błędy, ''code smells'', oraz zaproponuj refaktoryzację w celu poprawy czytelności i wydajności.', '- Nie używaj bibliotek zewnętrznych poza C++17 STL.\n- Zachowaj istniejącą funkcjonalność.\n- Odpowiadaj w języku polskim.', 'Zaproponuj zmiany w formacie ''diff''. Każdą sugestię opatrz zwięzłym uzasadnieniem.');
