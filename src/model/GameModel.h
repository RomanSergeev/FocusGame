#pragma once
#include <unordered_map>
#include "GameBoard.h"

struct GameRules {
    bool canJumpOverPits = true;
    bool canJumpThroughPoles = true;
    bool canPlaceAlliedReserve = false;
    bool canPlaceOnOwnTowers = true;
    bool canPlaceOnAllyTowers = false;
    bool canPlaceOnEnemyTowers = false;
    bool canExceedByPlacing = false;
    bool canPlaceOnPoles = false;
    int maxTowerHeight = 5;
    int maxReservePlaced = 1; // can be set to 0, so no reserve is ever placed
    int allyReserveTransferLimit = 2;

    GameRules() = default;
    bool canPlaceReserve() const { return maxReservePlaced > 0; }
    bool canTransferToAlly() const { return allyReserveTransferLimit > 0; }
};

class GameModel {
    static const int CANNOT_MOVE = -1;

    GameBoard board;
    std::vector<Player> players;
    std::unordered_map<PlayerSlot, std::vector<Checker>> trays;
    GameRules rules;
    int activePlayerIndex;
    //std::unordered_map<TeamSlot, std::vector<PlayerSlot>> alliances;

    //void fillAlliances();
    const Player* getPlayerBySlot(PlayerSlot slot) const;
    bool hasActiveAlly(const Player& player) const;
    bool isPlayerDefeated(const Player& player) const;
    bool canPlaceReserve(const Coord& cd, const Player& ofPlayer, int amount) const;
    bool canPerformAnyMove(const Player& player) const;
    bool canPlaceAnywhere(const Player& player) const;
    bool canTransferAnything(const Player& player) const;
    int canMove(const Coord& cFrom, const Coord& cTo) const; // shortest possible path length between the cells, or CANNOT_MOVE otherwise
    bool canTransferCheckers(const Player& toPlayer, int amount) const;
    int getTraySize(PlayerSlot ownedByPlayer, PlayerSlot ofPlayer) const;
    const Player& getCurrentPlayer() const { return players.at(activePlayerIndex); }
    //bool hasJumpableLineBetween(const Coord& from, const Coord& to, bool vertically = true) const;
public:
    GameModel(GameBoard&& board, std::vector<Player> players) : board(std::move(board)), players(std::move(players)) { /*fillAlliances();*/ }

    idxtype getRows() const { return board.sizes.x; }
    idxtype getColumns() const { return board.sizes.y; }
    const Cell& getCellAt(const Coord& cd) const { return board[cd]; }

    void updateDefeatedPlayers();
    bool placeReserve(const SessionKey& key, const Coord& cd, const Player& ofPlayer, int amount);
    bool move(const SessionKey& key, const Coord& from, const Coord& to);
    bool transferCheckers(const SessionKey& key, const Player& toPlayer, int amount);
    void transferMove(const SessionKey& key);
};