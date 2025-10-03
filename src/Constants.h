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

enum class TeamSlot {
    Team1, Team2, Team3, Team4,
    Team5, Team6, Team7, Team8,
    Spectator
};

enum class TurnType {
    Move,
    Place,
    Transfer
};

class AccessKey {
protected:
    AccessKey() {};
};

class EditorKey : public AccessKey {
    friend class BoardEditor;
    EditorKey() {};
};

class SessionKey : public AccessKey {
    friend class GameSession;
    SessionKey() {};
};