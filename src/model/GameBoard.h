#pragma once
#include "Cell.h"

class GameBoard {
    unsigned int rows;
    unsigned int columns;
    bool loopedHorizontally = false;
    bool loopedVertically = false;
    Cell** board; // TODO move to vector
public:
    GameBoard(unsigned int _rows, unsigned int _cols, bool loopedRows = false, bool loopedCols = false);
    ~GameBoard();

    unsigned int getRows() const { return rows; }
    unsigned int getColumns() const { return columns; }
    const Cell& getCellAt(int i, int j) const;

    void place(const EditorKey& key, unsigned int i, unsigned int j, Checker&& c);
    void markCell(const EditorKey& key, unsigned int i, unsigned int j, bool flagPlayable, bool flagJumpable);
    template<typename Condition>
    void markUnplayableByCondition(const EditorKey& key, Condition c) {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                if (c(rows, columns, i, j))
                    board[i][j].setPlayable(key, false);
    }
    template<typename Condition>
    void markUnjumpableByCondition(const EditorKey& key, Condition c) {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                if (c(rows, columns, i, j))
                    board[i][j].setJumpable(key, false);
    }
};