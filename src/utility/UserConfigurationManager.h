#pragma once
#include "model/UserConfiguration.h"

namespace UserConfigurationManager {
    const std::string userConfigurationDefaultFilePath = "UserSettings.ini";
    const std::string sectionNameDefaultColors = "Default Colors";
    const std::string sectionNameDefaultCamera = "Default Camera Settings";

    UserConfiguration loadConfiguration(const std::string& filename);
    void saveConfiguration(const UserConfiguration& config, const std::string& filename);
};