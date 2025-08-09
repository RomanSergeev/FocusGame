#pragma once
#include <string>
#include <unordered_map>
#include "Constants.h"
#include "model/GameBoard.h"
#include "utility/Utils.h"

class PlayerColorManager {
    std::unordered_map<unsigned int, Color> defaultColors;
    std::unordered_map<unsigned int, Color> playerColors;
public:
    void loadFromFile(const std::string& path);
    void loadPlayersFromGameConf(const GameBoard& board);
};