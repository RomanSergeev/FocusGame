#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include "UserConfigurationManager.h"
#include "Defaults.h"
#include "Utils.h"
#include "ini/INIReader.h"

using CamSettings = CameraController::CameraSettings;

uint8_t clampToByte(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return static_cast<uint8_t>(v);
}

void writeSection(std::ostringstream& oss, const std::string& sectionName) {
    oss << "[" << sectionName << "]" << std::endl;
}

void writePair(std::ostringstream& oss, const std::string& key, const std::string& value) {
    oss << key << "=" << value << std::endl;
}

Color parseColor(const std::string& str, bool& ok) {
    ok = true;
    auto iter = Color::namedColors.find(toLowerCase(str));
    if (iter != Color::namedColors.end()) return iter->second;

    std::string s = str;
    std::replace(s.begin(), s.end(), ',', ' '); // unify commas into spaces

    std::istringstream iss(s);
    std::vector<double> vals;
    double val;
    while (iss >> val) {
        vals.push_back(val);
    }

    if (vals.size() != 3) {
        ok = false;
        return Color::BLACK;
    }

    int r = static_cast<int>(std::round(vals[0]));
    int g = static_cast<int>(std::round(vals[1]));
    int b = static_cast<int>(std::round(vals[2]));

    return Color(clampToByte(r), clampToByte(g), clampToByte(b));
}

UserConfiguration UserConfigurationManager::loadConfiguration(const std::string& filename) {
    INIReader reader(filename);
    int error = reader.ParseError();
    if (error < 0) {
        Logger::logs("UserConfigurationManager::loadConfiguration: can't load file ", filename, ", falling back to defaults.");
        return UserConfiguration(defaultColors, Defaults::defaultCameraSettings);
    }

    std::unordered_map<PlayerSlot, Color> playerColors;
    CamSettings cameraSettings;
    bool parseOK = false;
    for (PlayerSlot slot : allPlayers) {
        std::string colorString = reader.Get(sectionNameDefaultColors, configNamesPlayer.at(slot), "");
        Color playerColor = parseColor(colorString, parseOK);
        if (!parseOK) playerColor = defaultColors.at(slot);
        playerColors[slot] = playerColor;
    }
    for (const auto& iter : Defaults::configNamesCameraFloat) {
        const std::string& name = iter.first;
        auto member = iter.second;
        cameraSettings.*member = reader.GetReal(sectionNameDefaultCamera, name, Defaults::defaultCameraSettings.*member);
    }
    for (const auto& iter : Defaults::configNamesCameraBool) {
        const std::string& name = iter.first;
        auto member = iter.second;
        cameraSettings.*member = reader.GetBoolean(sectionNameDefaultCamera, name, Defaults::defaultCameraSettings.*member);
    }

    return UserConfiguration(playerColors, cameraSettings);
}

void UserConfigurationManager::saveConfiguration(const UserConfiguration& config, const std::string& filename) {
    std::ostringstream oss;

    // [Default Colors]
    writeSection(oss, sectionNameDefaultColors);
    const auto& colors = config.getDefaultColors();
    for (const auto& iter: colors) {
        writePair(oss, configNamesPlayer.at(iter.first), iter.second.toString());
    }
    oss << std::endl;

    // [Default Camera Settings]
    writeSection(oss, sectionNameDefaultCamera);
    const CamSettings& map = config.getDefaultCameraSettings();
    for (const auto& iter : Defaults::configNamesCameraFloat)
        writePair(oss, iter.first, toString(map.*(iter.second)));
    for (const auto& iter : Defaults::configNamesCameraBool)
        writePair(oss, iter.first, toString(map.*(iter.second)));

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file) throw std::runtime_error("Could not open " + filename + " for writing");
    file << oss.str();
}