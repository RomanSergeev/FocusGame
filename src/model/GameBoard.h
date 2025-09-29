#pragma once
#include "Cell.h"

class GameBoard {
    friend class GameModel; // always direct ownership

    Coord sizes;
    bool loopedHorizontally = false;
    bool loopedVertically = false;
    std::vector<std::vector<Cell>> board;
    void placeChecker(const AccessKey& key, const Coord& cd, Checker&& c);
    const Cell& at(const Coord& cd) const { return operator[](cd); }
    Cell& at(const Coord& cd) { return board[cd.x][cd.y]; }
public:
    GameBoard(const Coord& dimensions, bool loopedRows = false, bool loopedCols = false);
    GameBoard(const GameBoard& gb) = delete;
    GameBoard& operator = (const GameBoard& gb) = delete;
    GameBoard(GameBoard&& gb) = default;
    GameBoard& operator = (GameBoard&& gb) = default;
    const Cell& operator[] (const Coord& cd) const;
    ~GameBoard() {};

    idxtype getRows() const { return sizes.x; }
    idxtype getColumns() const { return sizes.y; }

    bool validX(int x) const { return x >= 0 && x < sizes.x; }
    bool validY(int y) const { return y >= 0 && y < sizes.y; }
    bool validCoordinate(const Coord& cd) const;
    bool nextCoordinate(Coord& cd, JumpDirection jd, idxtype tetheredColumn = OVERLIMIT_SIZE) const; // returns false if cannot advance
    bool getCappedJumpDistanceInDirection(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int& maxDistance) const;
    int testMovementInDirectionWithTether(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int maxDistance, idxtype tether) const;
    void place(const AccessKey& key, const Coord& cd, Checker&& c) { placeChecker(key, cd, std::move(c)); }
    void markCell(const EditorKey& key, const Coord& cd, bool flagPlayable, bool flagJumpable);
    template<typename Condition>
    void markUnplayableByCondition(const EditorKey& key, Condition c) {
        for (idxtype i = 0; i < sizes.x; ++i)
            for (idxtype j = 0; j < sizes.y; ++j)
                if (c(sizes.x, sizes.y, i, j))
                    board[i][j].setPlayable(key, false);
    }
    template<typename Condition>
    void markUnjumpableByCondition(const EditorKey& key, Condition c) {
        for (idxtype i = 0; i < sizes.x; ++i)
            for (idxtype j = 0; j < sizes.y; ++j)
                if (c(sizes.x, sizes.y, i, j))
                    board[i][j].setJumpable(key, false);
    }
};