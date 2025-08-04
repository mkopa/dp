#include "DatabaseManager.h"
#include <iostream>
#include <stdexcept>
#include <SQLiteCpp/Transaction.h>

void DatabaseManager::seedInitialData() {
    std::cout << "Wypełnianie bazy danych danymi początkowymi (języki, prompty)..." << std::endl;
    m_db->exec("INSERT INTO languages (lang_code, name_native) VALUES ('en_US', 'English'), ('pl_PL', 'Polski');");
    m_db->exec("INSERT INTO ui_translations (key, lang_code, value) VALUES"
        "('themes_menu', 'en_US', 'Themes'),"
        "('themes_menu', 'pl_PL', 'Motywy'),"
        "('ui_language_menu', 'en_US', 'UI Language'),"
        "('ui_language_menu', 'pl_PL', 'Język UI'),"
        "('library_pane_title', 'en_US', 'Resource Library'),"
        "('library_pane_title', 'pl_PL', 'Biblioteka Zasobów'),"
        "('prompt_templates_header', 'en_US', 'Prompt Templates'),"
        "('prompt_templates_header', 'pl_PL', 'Szablony Promptów'),"
        "('role_header', 'en_US', 'Role / Persona'),"
        "('role_header', 'pl_PL', 'Rola / Persona'),"
        "('task_header', 'en_US', 'Task'),"
        "('task_header', 'pl_PL', 'Zadanie'),"
        "('constraints_header', 'en_US', 'Constraints'),"
        "('constraints_header', 'pl_PL', 'Ograniczenia'),"
        "('output_format_header', 'en_US', 'Output Format'),"
        "('output_format_header', 'pl_PL', 'Format Wyjściowy'),"
        "('load_code_button', 'en_US', 'Load code from directory'),"
        "('load_code_button', 'pl_PL', 'Wczytaj kod z katalogu'),"
        "('playground_pane_title', 'en_US', 'AI Playground'),"
        "('playground_pane_title', 'pl_PL', 'Plac Zabaw AI'),"
        "('copy_prompt_button', 'en_US', 'Copy final prompt'),"
        "('copy_prompt_button', 'pl_PL', 'Kopiuj gotowy prompt'),"
        "('model_response_tab', 'en_US', 'Model Response'),"
        "('model_response_tab', 'pl_PL', 'Odpowiedź Modelu'),"
        "('final_prompt_tab', 'en_US', 'Final Prompt (Preview)'),"
        "('final_prompt_tab', 'pl_PL', 'Finalny prompt (podgląd)'),"
        "('status_bar_tokens', 'en_US', 'Estimated tokens: %d'),"
        "('status_bar_tokens', 'pl_PL', 'Szacowana liczba tokenów: %d'),"
        "('status_bar_theme', 'en_US', 'Theme: %s'),"
        "('status_bar_theme', 'pl_PL', 'Motyw: %s'),"
        "('prompt_language_label', 'en_US', 'Prompt Content Language:'),"
        "('prompt_language_label', 'pl_PL', 'Język treści promptu:'),"
        "('refactor_prompt_name', 'en_US', 'C++ Refactoring'),"
        "('refactor_prompt_name', 'pl_PL', 'Refaktoryzacja C++');"
    );
    m_db->exec("INSERT INTO prompt_templates (name_key) VALUES ('refactor_prompt_name');");
    m_db->exec("INSERT INTO prompt_translations (template_id, lang_code, role, task, constraints, output_format) VALUES"
        "(1, 'en_US', 'You are an expert C++ programmer with 20 years of experience in low-latency, high-performance systems.', 'Analyze the code below. Identify potential bugs, code smells, and propose refactoring to improve readability and performance.', '- Do not use external libraries beyond the C++17 STL.\n- Preserve existing functionality.', 'Propose changes in a ''diff'' format. Provide a concise justification for each suggestion.'),"
        "(1, 'pl_PL', 'Jesteś ekspertem programistą C++ z 20-letnim doświadczeniem w systemach o niskim opóźnieniu i wysokiej wydajności.', 'Przeanalizuj poniższy kod. Zidentyfikuj potencjalne błędy, ''code smells'', oraz zaproponuj refaktoryzację w celu poprawy czytelności i wydajności.', '- Nie używaj bibliotek zewnętrznych poza C++17 STL.\n- Zachowaj istniejącą funkcjonalność.\n- Odpowiadaj w języku polskim.', 'Zaproponuj zmiany w formacie ''diff''. Każdą sugestię opatrz zwięzłym uzasadnieniem.');"
    );
}


DatabaseManager::DatabaseManager(const fs::path& db_path) : m_db_path(db_path) {
    try {
        // POPRAWKA LITERÓWKI
        m_db = std::make_unique<SQLite::Database>(m_db_path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        std::cout << "Baza danych otwarta pomyślnie: " << m_db_path.string() << std::endl;
        initializeSchema();
    } catch (const std::exception& e) {
        std::cerr << "BŁĄD KRYTYCZNY: Nie można otworzyć ani utworzyć bazy danych: " << m_db_path.string() << std::endl;
        std::cerr << "SQLiteCpp exception: " << e.what() << std::endl;
        throw;
    }
}

DatabaseManager::~DatabaseManager() {
    std::cout << "Baza danych zamknięta." << std::endl;
}

SQLite::Database& DatabaseManager::getDB() {
    if (!m_db) {
        throw std::runtime_error("Database is not open.");
    }
    return *m_db;
}

void DatabaseManager::initializeSchema() {
    SQLite::Transaction transaction(*m_db);

    int current_version = 0;
    if (m_db->tableExists("schema_version")) {
        current_version = m_db->execAndGet("SELECT version FROM schema_version");
    }

    if (current_version < 1) {
        std::cout << "Inicjalizacja nowego schematu bazy danych (v1)..." << std::endl;
        m_db->exec("CREATE TABLE schema_version (version INTEGER PRIMARY KEY)");
        m_db->exec("INSERT INTO schema_version (version) VALUES (0)");
        
        m_db->exec("CREATE TABLE configuration (key TEXT PRIMARY KEY NOT NULL, value TEXT NOT NULL)");
        
        m_db->exec("CREATE TABLE themes (theme_id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL UNIQUE)");
        m_db->exec("CREATE TABLE theme_colors (theme_id INTEGER NOT NULL, imgui_col_id INTEGER NOT NULL, r REAL NOT NULL, g REAL NOT NULL, b REAL NOT NULL, a REAL NOT NULL, FOREIGN KEY(theme_id) REFERENCES themes(theme_id), PRIMARY KEY(theme_id, imgui_col_id))");
        m_db->exec("CREATE TABLE theme_styles (theme_id INTEGER NOT NULL, imgui_style_var_id INTEGER NOT NULL, value_x REAL NOT NULL, value_y REAL NOT NULL, FOREIGN KEY(theme_id) REFERENCES themes(theme_id), PRIMARY KEY(theme_id, imgui_style_var_id))");
        
        m_db->exec("UPDATE schema_version SET version = 1");
        current_version = 1;
    }
    
    if (current_version < 2) {
        std::cout << "Aktualizacja schematu bazy danych do v2 (Lokalizacja)..." << std::endl;
        m_db->exec("CREATE TABLE languages (lang_code TEXT PRIMARY KEY NOT NULL, name_native TEXT NOT NULL)");
        m_db->exec("CREATE TABLE ui_translations (key TEXT NOT NULL, lang_code TEXT NOT NULL, value TEXT NOT NULL, PRIMARY KEY (key, lang_code), FOREIGN KEY(lang_code) REFERENCES languages(lang_code))");
        m_db->exec("CREATE TABLE prompt_templates (template_id INTEGER PRIMARY KEY AUTOINCREMENT, name_key TEXT NOT NULL UNIQUE)");
        m_db->exec("CREATE TABLE prompt_translations (template_id INTEGER NOT NULL, lang_code TEXT NOT NULL, role TEXT, task TEXT, constraints TEXT, output_format TEXT, PRIMARY KEY (template_id, lang_code), FOREIGN KEY(template_id) REFERENCES prompt_templates(template_id), FOREIGN KEY(lang_code) REFERENCES languages(lang_code))");
        
        seedInitialData();

        m_db->exec("UPDATE schema_version SET version = 2");
    }
    
    transaction.commit();
    std::cout << "Schemat bazy danych jest aktualny." << std::endl;
}