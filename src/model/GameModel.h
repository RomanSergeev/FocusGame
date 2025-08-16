#pragma once
#include <unordered_map>
#include <vector>
#include "Constants.h"
#include "GameBoard.h"
#include "model/Player.h"

class GameModel {
    GameBoard board;
    std::vector<Player> players;
    std::unordered_map<PlayerSlot, std::vector<Checker>> trays;
public:
    GameModel(GameBoard&& board, std::vector<Player> players) : board(std::move(board)), players(std::move(players)) {}

    unsigned int getRows() const { return board.getRows(); }
    unsigned int getColumns() const { return board.getColumns(); }
    const Cell& getCellAt(int i, int j) const { return board.getCellAt(i, j); }
    // should include GameBoard, vector<Player> players, be created in BoardEditor instead of GameBoard
    // corresponding classes: GameView, GameController
};