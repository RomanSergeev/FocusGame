#include <stdexcept>
#include "GameBoard.h"
#include "Checker.h"

void GameBoard::placeChecker(const Coord& cd, Checker&& c) {
    if (!validCoordinate(cd)) throw std::invalid_argument("GameBoard::place: coordinate out of range");
    // TODO add checks
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

void GameBoard::markCell(const EditorKey& key, const Coord& cd, bool flagPlayable, bool flagJumpable) {
    if (!validCoordinate(cd)) throw std::invalid_argument("GameBoard::markCell: coordinate out of range");
    at(cd).setPlayable(key, flagPlayable);
    at(cd).setJumpable(key, flagJumpable);
}