#pragma once
#include <optional>
#include "Constants.h"
#include "User.h"

enum class PlayerType {
    HUMAN_LOCAL,
    HUMAN_REMOTE,
    AI
};

class Player {
    friend class GameModel; // only GameModel can set player.defeated = true

    std::optional<User> userRef;
    std::string name;
    PlayerSlot slot;
    TeamSlot team;
    PlayerType type;
    bool defeated = false;
public:
    Player(const EditorKey& key, PlayerSlot slot, TeamSlot team, PlayerType type) : slot(slot), team(team), type(type) {}
    Player(const Player& p) = delete;
    Player& operator = (const Player& p) = delete;
    Player(Player&& p) = default;
    Player& operator = (Player&& p) = default;

    bool operator == (const Player& p) const { return userRef == p.userRef && slot == p.slot; }
    bool operator != (const Player& p) const { return !(*this == p); }

    PlayerSlot getSlot() const { return slot; }
    TeamSlot getTeam() const { return team; }
    bool isDefeated() const { return defeated; }
    bool isSpectator() const { return slot == PlayerSlot::Spectator; }
    bool sameTeam(const Player& player) const { return team == player.team && team != TeamSlot::Spectator; }
};