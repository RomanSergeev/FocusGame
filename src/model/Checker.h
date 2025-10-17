#pragma once
#include "Player.h"

struct Coord { // board coordinates wrapper
    static const Coord INVALID_COORD;

    idxtype x, y;
    Coord() = default;
    Coord(idxtype x, idxtype y) : x(x), y(y) {}
    Coord(const Coord& c) = default;
    Coord& operator = (const Coord& c) = default;
    bool operator == (const Coord& c) const { return x == c.x && y == c.y; }
    bool operator != (const Coord& c) const { return x != c.x || y != c.y; }
};

// Coord is used as a key of GameSession::allPossibleMoves map, as well as GameView::displayedBoard so it needs its hash defined
namespace std {
    template<>
    struct hash<Coord> {
        std::size_t operator() (const Coord& c) const noexcept {
            return std::hash<int>()(c.x ^ (c.y << 16));
        }
    };
}

class Checker {
    static int count;

    const Player* playerRef = nullptr;
    PlayerSlot inTrayOfPlayer = PlayerSlot::Spectator; // auxiliary members for drawing purposes, and quicker checker pinpointing
    Coord positionOnBoard = Coord::INVALID_COORD; // technically this is backwards dependency, but simplifies model-view binding
    void drop();
public:
    static int getInstancesCount() { return count; }

    Checker() { drop(); ++count; }
    Checker(const Player& p) : playerRef(&p) { ++count; }
    Checker(const Checker& c) = delete;
    Checker& operator = (const Checker& c) = delete;
    Checker(Checker&& c);
    Checker& operator = (Checker&& c);
    ~Checker() {
        --count;
    }

    const Player* getPlayerReference() const { return playerRef; }
    Coord getPositionOnBoard() const { return positionOnBoard; }
    bool isOnBoard() const { return positionOnBoard != Coord::INVALID_COORD; }
    bool isInTrayOf(PlayerSlot player) const { return !isOnBoard() && inTrayOfPlayer == player; }
    void putInTrayOf(const AccessKey& key, PlayerSlot player) { inTrayOfPlayer = player; positionOnBoard = Coord::INVALID_COORD; }
    void putOnBoard(const AccessKey& key, const Coord& cd) { positionOnBoard = cd; inTrayOfPlayer = PlayerSlot::Spectator; }
};