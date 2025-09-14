#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include "UserConfigurationManager.h"
#include "Constants.h"
#include "Utils.h"
#include "ini/INIReader.h"

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
    std::unordered_map<PlayerSlot, Color> playerColors;
    INIReader reader(filename);
    int error = reader.ParseError();
    if (error < 0) {
        std::cerr << "UserConfigurationManager::loadConfiguration: can't load file " << filename << ", falling back to defaults." << std::endl;
        return UserConfiguration(defaultColors);
    }
    bool parseOK = false;

    // TODO fill in a loop, move all section names to namespace (or this cpp) constants
    for (PlayerSlot slot : allPlayers) {
        std::string colorString = reader.Get("Default Colors", configPlayerNames.at(slot), "");
        Color playerColor = parseColor(colorString, parseOK);
        if (!parseOK) playerColor = defaultColors.at(slot);
        playerColors[slot] = playerColor;
    }

    return UserConfiguration(playerColors);
}

void UserConfigurationManager::saveConfiguration(const UserConfiguration& config, const std::string& filename) {
    std::ostringstream oss;
    writeSection(oss, "Default Colors");
    const auto& colors = config.getDefaultColors();
    for (auto iter = colors.begin(); iter != colors.end(); ++iter) {
        writePair(oss, configPlayerNames.at(iter->first), iter->second.toString());
    }

    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file) throw std::runtime_error("Could not open " + filename + " for writing");
    file << oss.str();
}