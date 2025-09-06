#pragma once
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"

using idxtype = unsigned char;

inline const glm::vec3 SPACE_ORIGIN(0.0f);
inline const float PI = glm::pi<float>();
const float TRACE_PRECISION = 1e-6f;

const int MAX_BOARD_SIZE = 100;
const int OVERLIMIT_SIZE = 101; // anything >= MAX_BOARD_SIZE

const int MAX_PLAYERS = 8;
const int DEFAULT_COLORS[MAX_PLAYERS * 3] {
    204, 204, 204, // white
     51,  51,  51, // black
    221,  34,  34, // red
     34,  34, 221, // blue
     34, 221,  34, // green
     34, 221, 221, // cyan
    221, 221,  34, // yellow
    221,  34, 221  // magenta
};

enum class PlayerSlot {
    Player1, Player2, Player3, Player4,
    Player5, Player6, Player7, Player8,
    Spectator
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