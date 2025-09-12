#pragma once
#include <string>
#include <unordered_map>
#include "model/GameBoard.h"

class PlayerColorManager {
    std::unordered_map<PlayerSlot, Color> defaultColors;
    std::unordered_map<PlayerSlot, Color> customColors;
public:
    void loadFromFile(const std::string& path);
    void loadPlayersFromGameConf(const GameBoard& board);
};