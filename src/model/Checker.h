#pragma once
#include "Player.h"

class Checker {
    static int count;

    const Player* playerRef;
    PlayerSlot inTrayOfPlayer = PlayerSlot::Spectator; // auxiliary member for drawing purposes
public:
    static int getInstancesCount() { return count; }

    Checker() : playerRef(nullptr) { ++count; }
    Checker(const Player& p) : playerRef(&p) { ++count; }
    Checker(const Checker& c) = delete;
    Checker& operator = (const Checker& c) = delete;
    Checker(Checker&& c);
    Checker& operator = (Checker&& c);
    ~Checker() { --count; }

    const Player* getPlayerReference() const { return playerRef; }
    bool isOnBoard() const { return inTrayOfPlayer == PlayerSlot::Spectator; }
    bool isInTrayOf(PlayerSlot player) const { return !isOnBoard() && inTrayOfPlayer == player; }
    void putInTrayOf(const AccessKey& key, PlayerSlot player) { inTrayOfPlayer = player; }
    void removeFromTray(const AccessKey& key) { inTrayOfPlayer = PlayerSlot::Spectator; }
};