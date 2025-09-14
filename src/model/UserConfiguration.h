#pragma once
#include <unordered_map>
#include "Color.h"
#include "Constants.h"

class UserConfiguration {
    std::unordered_map<PlayerSlot, Color> defaultColors;
public:
    UserConfiguration(const std::unordered_map<PlayerSlot, Color>& colors) : defaultColors(colors) {}
    const std::unordered_map<PlayerSlot, Color>& getDefaultColors() const { return defaultColors; }
};