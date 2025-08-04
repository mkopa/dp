#pragma once

#include "AppState.h"
#include <string>

namespace ConfigManager {
    void LoadConfig(AppState& state);
    void SaveConfig(const AppState& state);
}
