#pragma once
#include "Constants.h"
#include "Player.h"

class Checker {
    static int count;

    const Player* playerRef;
public:
    static int getInstancesCount() { return count; }

    Checker() : playerRef(nullptr) { ++count; }
    Checker(const Player& p) : playerRef(&p) { ++count; }
    Checker(const Checker& c) = delete;
    Checker& operator = (const Checker& c) = delete;
    Checker(Checker&& c) : playerRef(c.playerRef) { c.playerRef = nullptr; ++count; }
    Checker& operator = (Checker&& c) {
        playerRef = c.playerRef;
        c.playerRef = nullptr;
        return *this;
    }
    ~Checker() { --count; }

    const Player* getPlayerReference() const { return playerRef; }
};