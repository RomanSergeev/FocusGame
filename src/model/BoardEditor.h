#pragma once
#include "GameBoard.h"

class BoardEditor {
    GameBoard& board;
    EditorKey key;
public:
    static GameBoard createBoard8x8Focus();
    static GameBoard createBoard8x8Chess();

    BoardEditor(GameBoard& gb) : board(gb) {}
};