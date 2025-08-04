#pragma once
#include <string>
#include <vector>
#include <SQLiteCpp/Database.h>

namespace ThemeManager
{
    void InitializeAndSeedThemes(SQLite::Database& db);
    std::vector<std::string> GetThemeNames(SQLite::Database& db);
    void ApplyTheme(const std::string& themeName, SQLite::Database& db);
}