#include "Checker.h"
#include "Constants.h"

int Checker::count = 0;

void Checker::drop() {
    playerRef = nullptr;
    inTrayOfPlayer = PlayerSlot::Spectator;
    positionOnBoard = Coord::INVALID_COORD;
}

Checker::Checker(Checker&& c) : playerRef(c.playerRef), inTrayOfPlayer(c.inTrayOfPlayer), positionOnBoard(c.positionOnBoard) {
    c.drop();
    ++count;
}

Checker& Checker::operator = (Checker&& c) {
    playerRef = c.playerRef;
    inTrayOfPlayer = c.inTrayOfPlayer;
    positionOnBoard = c.positionOnBoard;
    c.drop();
    return *this;
}