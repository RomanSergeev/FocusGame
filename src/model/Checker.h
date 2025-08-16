#pragma once
#include "Constants.h"
#include "Player.h"

class Checker {
    const Player* playerRef;
public:
    Checker() : playerRef(nullptr) {}
    Checker(const Player& p) : playerRef(&p) {}
    Checker(const Checker& c) = delete;
    Checker& operator = (const Checker& c) = delete;
    Checker(Checker&& c) : playerRef(c.playerRef) { c.playerRef = nullptr; }
    Checker& operator = (Checker&& c) {
        playerRef = c.playerRef;
        c.playerRef = nullptr;
        return *this;
    }

    const Player* getPlayerReference() const { return playerRef; }
};