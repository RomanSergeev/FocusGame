#include <stdexcept>
#include "GameBoard.h"

void GameBoard::placeChecker(const AccessKey& key, const Coord& cd, Checker&& c) {
    if (!validCoordinate(cd)) throw std::invalid_argument("GameBoard::place: coordinate out of range");
    c.removeFromTray(key);
    at(cd).append(std::move(c));
}

GameBoard::GameBoard(const Coord& dimensions, bool loopedRows, bool loopedCols) :
sizes(dimensions), loopedHorizontally(loopedRows), loopedVertically(loopedCols) {
    if (sizes.x == 0 || sizes.y == 0) {
        throw std::invalid_argument("GameBoard::GameBoard: attempt to create board of zero size");
    }
    board.resize(sizes.x);
    for (idxtype i = 0; i < sizes.x; ++i)
        board[i].resize(sizes.y);
}

const Cell& GameBoard::operator [] (const Coord& cd) const {
    if (!validCoordinate(cd)) throw std::invalid_argument("GameBoard::operator[]: coordinate out of range");
    return board[cd.x][cd.y];
}

bool GameBoard::validCoordinate(const Coord& cd) const {
    return cd.x >= 0 && cd.x < sizes.x && cd.y >= 0 && cd.y < sizes.y;
}

bool GameBoard::nextCoordinate(Coord& cd, JumpDirection jd, idxtype tetheredColumn) const {
    int dx = getDeltaX(jd), dy = getDeltaY(jd);
    int nextX = cd.x + dx, nextY = cd.y + dy;
    if (!validX(nextX)) {
        if (!loopedHorizontally) return false;
        nextX = (nextX + sizes.x) % sizes.x;
    }
    if (!validY(nextY)) {
        if (!loopedVertically) return false;
        nextY = (nextY + sizes.y) % sizes.y;
    }
    if (at(cd).isPole()) {
        if (isHorizontal(jd)) return false;
        nextY = tetheredColumn; // two poles cannot stand together
    }
    if (board[nextX][0].isPole()) nextY = 0;
    cd.x = nextX;
    cd.y = nextY;
    return true;
}

bool GameBoard::getCappedJumpDistanceInDirection(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int& maxDistance) const {
    // from is owned by currentPlayer and has checkers on it; from and to are playable; indices are valid and different
    if (!at(cFrom).isPole()) return testMovementInDirectionWithTether(cFrom, cTo, jd, maxDistance, cFrom.y);
    if (isHorizontal(jd)) return false; // poles can only be horizontal - thus, horizontal movement is forbidden
    for (idxtype j = 0; j < sizes.y; ++j) {
        int distance = testMovementInDirectionWithTether(cFrom, cTo, jd, maxDistance, j);
        if (distance < maxDistance) { maxDistance = distance; return true; }
    }
    // we've scanned through all the columns and couldn't reach the destination
    return false;
}

int GameBoard::testMovementInDirectionWithTether(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int maxDistance, idxtype tether) const {
    Coord c = cFrom;
    int totalDistance = 0;
    while (totalDistance < maxDistance) {
        bool canAdvance = nextCoordinate(c, jd, tether); // coord changes (tries to advance)
        if (!canAdvance) return OVERLIMIT_SIZE;
        ++totalDistance;
        if (c == cTo) return totalDistance; // if we've reached the destination
        if (!at(c).isJumpableOver()) return OVERLIMIT_SIZE; // cell to travel through is not jumpable over
        if (at(c).isPole()) jd = reverseDirection(jd);
    }
    return OVERLIMIT_SIZE;
}

void GameBoard::markCell(const EditorKey& key, const Coord& cd, bool flagPlayable, bool flagJumpable) {
    if (!validCoordinate(cd)) throw std::invalid_argument("GameBoard::markCell: coordinate out of range");
    at(cd).setPlayable(key, flagPlayable);
    at(cd).setJumpable(key, flagJumpable);
}