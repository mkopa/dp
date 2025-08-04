#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <SQLiteCpp/Database.h>

namespace fs = std::filesystem;

class DatabaseManager {
public:
    DatabaseManager(const fs::path& db_path);
    ~DatabaseManager();

    SQLite::Database& getDB();

private:
    void initializeSchema();
    void seedInitialData(); // <-- DODANO BRAKUJĄCĄ DEKLARACJĘ

    std::unique_ptr<SQLite::Database> m_db;
    fs::path m_db_path;
};