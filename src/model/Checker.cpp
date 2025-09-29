#include "Checker.h"

int Checker::count = 0;

Checker::Checker(Checker&& c) : playerRef(c.playerRef), inTrayOfPlayer(c.inTrayOfPlayer) {
    c.playerRef = nullptr;
    c.inTrayOfPlayer = PlayerSlot::Spectator;
    ++count;
}

Checker& Checker::operator = (Checker&& c) {
    playerRef = c.playerRef;
    c.playerRef = nullptr;
    return *this;
}