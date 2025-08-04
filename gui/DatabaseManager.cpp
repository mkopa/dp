#include "DatabaseManager.h"
#include <iostream>
#include <stdexcept>
#include <SQLiteCpp/Transaction.h>

DatabaseManager::DatabaseManager(const fs::path& db_path) : m_db_path(db_path) {
    try {
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
    bool themes_table_exists = m_db->tableExists("themes");

    if (!themes_table_exists) {
        std::cout << "Inicjalizacja nowego schematu bazy danych..." << std::endl;
        try {
            SQLite::Transaction transaction(*m_db);
            m_db->exec("CREATE TABLE schema_version (version INTEGER PRIMARY KEY)");
            m_db->exec("INSERT INTO schema_version (version) VALUES (1)");
            m_db->exec("CREATE TABLE configuration (key TEXT PRIMARY KEY NOT NULL, value TEXT NOT NULL)");
            m_db->exec("CREATE TABLE themes (theme_id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL UNIQUE)");
            m_db->exec("CREATE TABLE theme_colors (theme_id INTEGER NOT NULL, imgui_col_id INTEGER NOT NULL, r REAL NOT NULL, g REAL NOT NULL, b REAL NOT NULL, a REAL NOT NULL, FOREIGN KEY(theme_id) REFERENCES themes(theme_id), PRIMARY KEY(theme_id, imgui_col_id))");
            m_db->exec("CREATE TABLE theme_styles (theme_id INTEGER NOT NULL, imgui_style_var_id INTEGER NOT NULL, value_x REAL NOT NULL, value_y REAL NOT NULL, FOREIGN KEY(theme_id) REFERENCES themes(theme_id), PRIMARY KEY(theme_id, imgui_style_var_id))");
            transaction.commit();
            std::cout << "Schemat bazy danych został pomyślnie utworzony." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas tworzenia schematu bazy danych: " << e.what() << std::endl;
            throw;
        }
    } else {
        std::cout << "Istniejący schemat bazy danych został znaleziony." << std::endl;
    }
}