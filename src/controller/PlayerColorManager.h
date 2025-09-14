#pragma once
#include <string>
#include <unordered_map>
#include "model/GameBoard.h"
#include "model/UserConfiguration.h"

class PlayerColorManager {
    std::unordered_map<PlayerSlot, Color> defaultColors;
    std::unordered_map<PlayerSlot, Color> customColors;
public:
    void loadConfiguration(const UserConfiguration& conf);
    void loadPlayersFromGameConf(const GameBoard& board);
};