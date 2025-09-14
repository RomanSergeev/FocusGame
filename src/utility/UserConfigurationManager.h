#pragma once
#include "model/UserConfiguration.h"

namespace UserConfigurationManager {
    const std::string UserConfigurationDefaultFilePath = "UserSettings.ini";

    UserConfiguration loadConfiguration(const std::string& filename);
    void saveConfiguration(const UserConfiguration& config, const std::string& filename);
};