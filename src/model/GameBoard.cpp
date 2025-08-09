#include <stdexcept>
#include "GameBoard.h"
#include "Checker.h"

GameBoard::GameBoard(unsigned int _rows, unsigned int _cols, bool loopedRows, bool loopedCols) :
rows(_rows), columns(_cols), loopedHorizontally(loopedRows), loopedVertically(loopedCols) {
    if (rows == 0 || columns == 0) {
        throw std::invalid_argument("GameBoard::GameBoard: attempt to create board of zero size");
    }
    int i;
    try {
        board = new Cell*[rows];
        for (i = 0; i < rows; ++i)
            board[i] = new Cell[columns];
    }
    catch (std::bad_alloc) {
        for (int j = 0; j < i; ++j)
            delete[] board[j];
        delete[] board;
        throw;
    }
}

GameBoard::~GameBoard() {
    for (int i = 0; i < rows; ++i)
        delete[] board[i];
    delete[] board;
}

const Cell& GameBoard::getCellAt(int i, int j) const {
    if (i < 0 || i >= rows)
        throw std::invalid_argument("GameBoard::getCellAt: argument i out of range");
    if (j < 0 || j >= columns)
        throw std::invalid_argument("GameBoard::getCellAt: argument j out of range");
    return board[i][j];
}

void GameBoard::place(const EditorKey& key, unsigned int i, unsigned int j, Checker&& c) {
    if (i < 0 || i >= rows)
        throw std::invalid_argument("GameBoard::place: argument i out of range");
    if (j < 0 || j >= columns)
        throw std::invalid_argument("GameBoard::place: argument j out of range");
    // TODO add checks
    board[i][j].append(std::move(c));
}

void GameBoard::markCell(const EditorKey& key, unsigned int i, unsigned int j, bool flagPlayable, bool flagJumpable) {
    if (i < 0 || i >= rows)
        throw std::invalid_argument("GameBoard::markCell: argument i out of range");
    if (j < 0 || j >= columns)
        throw std::invalid_argument("GameBoard::markCell: argument j out of range");
    board[i][j].setPlayable(key, flagPlayable);
    board[i][j].setJumpable(key, flagJumpable);
}