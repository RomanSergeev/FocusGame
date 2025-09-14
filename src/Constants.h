#pragma once
#include "Color.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"

using idxtype = unsigned char;

inline const glm::vec3 SPACE_ORIGIN(0.0f);
inline const float PI = glm::pi<float>();
const float TRACE_PRECISION = 1e-6f;

const int MAX_BOARD_SIZE = 100;
const int OVERLIMIT_SIZE = 101; // anything >= MAX_BOARD_SIZE

const int MAX_PLAYERS = 8;

enum class PlayerSlot {
    Player1, Player2, Player3, Player4,
    Player5, Player6, Player7, Player8,
    Spectator
};

constexpr std::array<PlayerSlot, MAX_PLAYERS> allPlayers = {
    PlayerSlot::Player1,
    PlayerSlot::Player2,
    PlayerSlot::Player3,
    PlayerSlot::Player4,
    PlayerSlot::Player5,
    PlayerSlot::Player6,
    PlayerSlot::Player7,
    PlayerSlot::Player8
};

const std::unordered_map<PlayerSlot, Color> defaultColors {
    { PlayerSlot::Player1, {204, 204, 204} }, // white
    { PlayerSlot::Player2, { 51,  51,  51} }, // black
    { PlayerSlot::Player3, {221,  34,  34} }, // red
    { PlayerSlot::Player4, { 34,  34, 221} }, // blue
    { PlayerSlot::Player5, { 34, 221,  34} }, // green
    { PlayerSlot::Player6, { 34, 221, 221} }, // cyan
    { PlayerSlot::Player7, {221, 221,  34} }, // yellow
    { PlayerSlot::Player8, {221,  34, 221} }  // magenta
};

const std::unordered_map<PlayerSlot, std::string> configPlayerNames {
    { PlayerSlot::Player1, "player1" },
    { PlayerSlot::Player2, "player2" },
    { PlayerSlot::Player3, "player3" },
    { PlayerSlot::Player4, "player4" },
    { PlayerSlot::Player5, "player5" },
    { PlayerSlot::Player6, "player6" },
    { PlayerSlot::Player7, "player7" },
    { PlayerSlot::Player8, "player8" }
};

enum class TeamSlot {
    Team1, Team2, Team3, Team4,
    Team5, Team6, Team7, Team8,
    Spectator
};

class EditorKey {
    friend class BoardEditor;
    EditorKey() = default;
};

class SessionKey {
    friend class GameController;
    SessionKey() = default;
};