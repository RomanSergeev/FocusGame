#pragma once
#include <map>
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
    std::map<PlayerSlot, std::vector<Checker>> trays;
    GameRules rules;
    int activePlayerIndex;
    
    const Player* getPlayerBySlot(PlayerSlot slot) const;
    bool hasActiveAlly(const Player& player) const;
    bool isPlayerDefeated(const Player& player) const;
    bool canPerformAnyMove(const Player& player) const;
    bool canPlaceAnywhere(const Player& player) const;
    bool canTransferAnything(const Player& player) const;
    bool canPlaceReserve(const Coord& cd, const Player& ofPlayer, int amount) const;
    bool canTransferCheckers(const Player& toPlayer, int amount) const;
    int getTraySize(PlayerSlot ownedByPlayer, PlayerSlot ofPlayer) const;
    //bool hasJumpableLineBetween(const Coord& from, const Coord& to, bool vertically = true) const;

    void putExcessToTray(const SessionKey& key, const Coord& from);
    bool move(const SessionKey& key, const Coord& from, const Coord& to);
    bool placeReserve(const SessionKey& key, const Coord& cd, const Player& ofPlayer, int amount);
    bool transferCheckers(const SessionKey& key, const Player& toPlayer, int amount);
    void transferMove();
public:
    struct Turn {
        TurnType type; // Move, Place, Transfer
        PlayerSlot assocPlayer; // Place, Transfer
        int checkerAmount; // Place, Transfer
        Coord from; // Move
        Coord to; // Move, Place
    };

    struct MovePossibility {
        std::vector<std::pair<const Cell*, int>> canGoTo; // cell references and corresponding distances
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

    int canMove(const Coord& cFrom, const Coord& cTo) const; // shortest possible path length between the cells, or CANNOT_MOVE otherwise. Used in GameSession - made public
    void TEMPputCheckerIntoTray(const EditorKey& key, const Player& p, Checker&& c);
    const Player& TEMPgetPlayer(int index) const { return players.at(index); }
    void updateDefeatedPlayers();
    bool makeTurn(const SessionKey& key, const Turn& turn);
    bool isGameOver() const;
    MovePossibility getPossibleMovesFor(const Coord& cd) const;
};