
#include <optional>
#pragma once
#include <optional>
#include "User.h"
#include "utility/Utils.h"

enum class PlayerType {
    HUMAN_LOCAL,
    HUMAN_REMOTE,
    AI
};

class Player {
    std::optional<User> userRef;
    unsigned int id; // local, assigned on game start
    std::string name;
    PlayerType type;
    bool active; // either player or spectator
public:
    unsigned int getID() const { return id; }
};