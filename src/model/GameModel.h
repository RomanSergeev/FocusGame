#pragma once
#include <map>
#include "Cell.h"
#include "Constants.h"
#include "GameBoard.h"

class GameModel {
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

    static const int CANNOT_MOVE = -1;

    GameBoard board;
    std::vector<Player> players;
    std::map<PlayerSlot, CheckerContainer> trays;
    GameRules rules;
    int activePlayerIndex;
    bool gameInProgress; // transferMove should be the only place where this changes

    const Player* getPlayerBySlot(PlayerSlot slot) const;
    bool hasActiveAlly(const Player& player) const;
    bool isPlayerDefeated(const Player& player) const;
    bool canPerformAnyMove(const Player& player) const;
    bool canPlaceAnywhere(const Player& player) const;
    bool canTransferAnything(const Player& player) const;
    bool canTransferCheckers(const Player& toPlayer, int amount) const;
    int getTraySize(PlayerSlot ownedByPlayer, PlayerSlot ofPlayer) const;

    void updateDefeatedPlayers(); // can be public, but doesn't need to be
    // either to current player's tray (to == INVALID_COORD), or a cell (to is valid)
    void moveAndAppendCheckers(const SessionKey& key, const Coord& from, const Coord& to, int srcStart, int srcEnd);
    void putExcessToTray(const SessionKey& key, const Coord& from);
    bool move(const SessionKey& key, const Coord& from, const Coord& to);
    bool placeReserve(const SessionKey& key, const Coord& cd, const Player& ofPlayer, int amount);
    bool transferCheckers(const SessionKey& key, const Player& toPlayer, int amount);
    void transferMove(const SessionKey& key);
public:
    struct Turn {
        static Turn constructTurnMove(const Coord& from, const Coord& to);
        static Turn constructTurnPlace(PlayerSlot associatedPlayer, int checkerAmount, const Coord& to);
        static Turn constructTurnTransfer(PlayerSlot associatedPlayer, int checkerAmount);
        
        TurnType type; // Move, Place, Transfer
        PlayerSlot assocPlayer; // Place, Transfer
        int checkerAmount; // Place, Transfer
        Coord from; // Move
        Coord to; // Move, Place
    };

    struct MovePossibility {
        std::vector<std::pair<Coord, int>> canGoTo; // cell references and corresponding distances
        int maxReservePlaced;
        MovePossibility() = default;
        void clear() { canGoTo.clear(); maxReservePlaced = 0; }
    };

    GameModel(GameBoard&& board, std::vector<Player> players);

    const Coord& getSizes() const { return board.sizes; }
    idxtype getRows() const { return board.sizes.x; }
    idxtype getColumns() const { return board.sizes.y; }
    const Cell& getCellAt(const Coord& cd) const { return board[cd]; }
    const auto& getTrayData() const { return trays; }
    const Player& getCurrentPlayer() const { return players.at(activePlayerIndex); }
    int getMaximumPlacedReserve() const { return rules.maxReservePlaced; }
    int getCheckersTransferLimit() const { return rules.allyReserveTransferLimit; }

    bool canPlaceReserve(const Coord& cd, const Player& ofPlayer, int amount) const;
    int canMove(const Coord& cFrom, const Coord& cTo) const; // shortest possible path length between the cells, or CANNOT_MOVE otherwise. Used in GameSession - made public
    void TEMPputCheckerIntoTray(const EditorKey& key, const Player& p, Checker&& c);
    const Player& TEMPgetPlayer(int index) const { return players.at(index); }
    void start(const SessionKey& key);
    bool makeTurn(const SessionKey& key, const Turn& turn);
    bool isGameOver() const { return !gameInProgress; }
    MovePossibility getPossibleMovesFor(const Coord& cd) const;
    bool isSelectableChecker(const Checker& c) const;
    bool canSelectNCheckersOfPlayer(int amount, const Player& ofPlayer) const;
};