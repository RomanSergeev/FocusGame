#pragma once
#include "GameModel.h"

class BoardEditor {
    GameBoard& board;
    EditorKey key;
public:
    static GameModel createBoard8x8Focus();
    static GameModel createBoard8x8Chess();

    BoardEditor(GameBoard& gb) : board(gb) {}
};