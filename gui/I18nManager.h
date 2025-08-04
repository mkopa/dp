#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <SQLiteCpp/Database.h>

// Struktura do przechowywania treści promptu
struct PromptContent {
    std::string role;
    std::string task;
    std::string constraints;
    std::string output_format;
};

class I18nManager {
public:
    // Singleton access
    static I18nManager& GetInstance();

    void Initialize(SQLite::Database& db);
    
    // Zarządzanie językiem UI
    void SetUILanguage(const std::string& lang_code);
    const std::string& GetCurrentUILanguage() const;
    const char* L(const char* key); // Główna funkcja tłumacząca UI

    // Pobieranie dostępnych języków
    const std::map<std::string, std::string>& GetAvailableUILanguages() const;
    const std::map<std::string, std::string>& GetAvailablePromptLanguages() const;

    // Zarządzanie promptami
    std::vector<std::string> GetPromptTemplateNames(const std::string& ui_lang_code);
    PromptContent GetPromptContent(const std::string& name_key, const std::string& prompt_lang_code);

private:
    I18nManager() = default;
    ~I18nManager() = default;
    I18nManager(const I18nManager&) = delete;
    I18nManager& operator=(const I18nManager&) = delete;

    void LoadStrings(const std::string& lang_code);

    SQLite::Database* m_db = nullptr;
    std::string m_current_ui_lang_code = "en_US";
    std::map<std::string, std::string> m_strings;
    std::map<std::string, std::string> m_available_ui_languages;
    std::map<std::string, std::string> m_available_prompt_languages;
};

// Skrót dla łatwiejszego dostępu
const char* L(const char* key);