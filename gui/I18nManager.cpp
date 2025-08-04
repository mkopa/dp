#include "I18nManager.h"
#include <SQLiteCpp/Statement.h>
#include <iostream>

I18nManager& I18nManager::GetInstance() {
    static I18nManager instance;
    return instance;
}

void I18nManager::Initialize(SQLite::Database& db) {
    m_db = &db;

    // Wczytaj dostępne języki
    try {
        SQLite::Statement query_langs(*m_db, "SELECT lang_code, name_native FROM languages");
        while (query_langs.executeStep()) {
            std::string code = query_langs.getColumn(0);
            std::string name = query_langs.getColumn(1);
            m_available_ui_languages[code] = name;
            m_available_prompt_languages[code] = name; // Na razie te same
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd wczytywania języków: " << e.what() << std::endl;
    }

    SetUILanguage("en_US"); // Domyślny język
}

void I18nManager::SetUILanguage(const std::string& lang_code) {
    if (m_available_ui_languages.count(lang_code)) {
        m_current_ui_lang_code = lang_code;
        LoadStrings(lang_code);
    }
}

const std::string& I18nManager::GetCurrentUILanguage() const {
    return m_current_ui_lang_code;
}

void I18nManager::LoadStrings(const std::string& lang_code) {
    m_strings.clear();
    try {
        SQLite::Statement query(*m_db, "SELECT key, value FROM ui_translations WHERE lang_code = ?");
        query.bind(1, lang_code);
        while (query.executeStep()) {
            m_strings[query.getColumn(0).getString()] = query.getColumn(1).getString();
        }
        std::cout << "Załadowano " << m_strings.size() << " tłumaczeń dla języka " << lang_code << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Błąd wczytywania tłumaczeń dla " << lang_code << ": " << e.what() << std::endl;
    }
}

const char* I18nManager::L(const char* key) {
    auto it = m_strings.find(key);
    if (it != m_strings.end()) {
        return it->second.c_str();
    }
    // Zwróć klucz jako fallback, aby łatwo znaleźć brakujące tłumaczenia
    return key;
}

const std::map<std::string, std::string>& I18nManager::GetAvailableUILanguages() const {
    return m_available_ui_languages;
}

const std::map<std::string, std::string>& I18nManager::GetAvailablePromptLanguages() const {
    return m_available_prompt_languages;
}

std::vector<std::string> I18nManager::GetPromptTemplateNames(const std::string& ui_lang_code) {
    std::vector<std::string> names;
    try {
        SQLite::Statement query(*m_db, "SELECT T1.value FROM ui_translations T1 INNER JOIN prompt_templates T2 ON T1.key = T2.name_key WHERE T1.lang_code = ? ORDER BY T2.template_id");
        query.bind(1, ui_lang_code);
        while (query.executeStep()) {
            names.push_back(query.getColumn(0).getString());
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd pobierania nazw szablonów: " << e.what() << std::endl;
    }
    return names;
}

PromptContent I18nManager::GetPromptContent(const std::string& name_key, const std::string& prompt_lang_code) {
    PromptContent content;
    try {
        SQLite::Statement query(*m_db, "SELECT T2.role, T2.task, T2.constraints, T2.output_format FROM prompt_templates T1 INNER JOIN prompt_translations T2 ON T1.template_id = T2.template_id WHERE T1.name_key = ? AND T2.lang_code = ?");
        query.bind(1, name_key);
        query.bind(2, prompt_lang_code);
        if (query.executeStep()) {
            content.role = query.getColumn(0).getString();
            content.task = query.getColumn(1).getString();
            content.constraints = query.getColumn(2).getString();
            content.output_format = query.getColumn(3).getString();
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd pobierania treści promptu: " << e.what() << std::endl;
    }
    return content;
}

// Globalna funkcja pomocnicza
const char* L(const char* key) {
    return I18nManager::GetInstance().L(key);
}