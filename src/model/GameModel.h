#pragma once
#include <unordered_map>
#include <vector>
#include "Constants.h"
#include "GameBoard.h"
#include "model/Player.h"

struct GameRules {
    bool canJumpOverPits = true;
    bool canJumpThroughPoles = true;
    bool canPlaceAlliedReserve = false;
    bool canPlaceOnOwnTowers = true;
    bool canPlaceOnAllyTowers = false;
    bool canPlaceOnEnemyTowers = false;
    int maxTowerHeight = 5;
    int maxReservePlaced = 1;
    int allyReserveTransferLimit = 2;

    bool canTransferToAlly() const { return allyReserveTransferLimit > 0; }
};

class GameModel {
    GameBoard board;
    std::vector<Player> players;
    std::unordered_map<PlayerSlot, std::vector<Checker>> trays;
    int activePlayerIndex; // TODO introduce
public:
    GameModel(GameBoard&& board, std::vector<Player> players) : board(std::move(board)), players(std::move(players)) {}

    unsigned int getRows() const { return board.getRows(); }
    unsigned int getColumns() const { return board.getColumns(); }
    const Cell& getCellAt(int i, int j) const { return board.getCellAt(i, j); }

    bool playerDefeated(PlayerSlot player) const;
    bool canPlaceReserve(int i, int j, PlayerSlot fromPlayer, int amount) const;
    bool canMove(int iFrom, int jFrom, int iTo, int jTo) const;
    bool placeReserve(SessionKey key, int i, int j, PlayerSlot fromPlayer, int amount);
    bool move(SessionKey key, int iFrom, int jFrom, int iTo, int jTo);
    // should include GameBoard, vector<Player> players, be created in BoardEditor instead of GameBoard
    // corresponding classes: GameView, GameController
};