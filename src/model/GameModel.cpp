#include <iterator>
#include "Constants.h"
#include "GameModel.h"
#include "Player.h"
#include "utility/Utils.h"

GameModel::Turn GameModel::Turn::constructTurnMove(const Coord& from, const Coord& to) {
    Turn result;
    result.type = TurnType::Move;
    result.from = from;
    result.to = to;
    return result;
}

GameModel::Turn GameModel::Turn::constructTurnPlace(PlayerSlot associatedPlayer, int checkerAmount, const Coord &to) {
    Turn result;
    result.type = TurnType::Place;
    result.assocPlayer = associatedPlayer;
    result.checkerAmount = checkerAmount;
    result.to = to;
    return result;
}

GameModel::Turn GameModel::Turn::constructTurnTransfer(PlayerSlot associatedPlayer, int checkerAmount) {
    Turn result;
    result.type = TurnType::Transfer;
    result.assocPlayer = associatedPlayer;
    result.checkerAmount = checkerAmount;
    return result;
}

const Player* GameModel::getPlayerBySlot(PlayerSlot slot) const {
    for (const Player& player : players) {
        if (player.slot == slot) return &player;
    }
    return nullptr;
}

bool GameModel::hasActiveAlly(const Player& player) const {
    if (player.isSpectator()) return false;
    for (const Player& p : players) {
        if (p != player && p.sameTeam(player) && !p.defeated) return true;
    }
    return false;
}

bool GameModel::isPlayerDefeated(const Player& player) const {
    if (player.isSpectator()) return false; // skip spectator
    return !canTransferAnything(player) && !canPlaceAnywhere(player) && !canPerformAnyMove(player);
}

bool GameModel::canPerformAnyMove(const Player& player) const {
    for (idxtype i = 0; i < board.sizes.x; ++i)
        for (idxtype j = 0; j < board.sizes.y; ++j) {
            Coord cd = {i, j};
            const Cell& cell = board.at(cd);
            if (cell.isPole()) j = board.sizes.y;
            // simplified for now - assume any player's tower can go at least somewhere
            // not true, generally speaking
            if (cell.getOwnership() == &player) return true;
        }
    return false;
}

bool GameModel::canPlaceAnywhere(const Player& player) const {
    bool anyReserve = false;
    const std::vector<Checker>& tray = trays.at(player.slot);
    for (const Checker& c : tray) {
        const Player* p = c.getPlayerReference();
        if (isNull(p, "GameModel::canPlaceAnywhere: found Cell's null Player reference")) continue;
        if (p == &player || (rules.canPlaceAlliedReserve && player.sameTeam(*p))) {
            anyReserve = true;
            break;
        }
    }
    if (!anyReserve) return false;

    // check if we can place our reserve anywhere
    for (idxtype i = 0; i < board.sizes.x; ++i)
        for (idxtype j = 0; j < board.sizes.y; ++j) {
            const Cell& cell = board.at({i, j});
            if (cell.isPole()) {
                j = board.sizes.y; // crutchy way to skip the entire row, if its first element is a pole
                if (!rules.canPlaceOnPoles) continue;
            }
            if (!cell.isPlayable()) continue;
            const Player* owner = cell.getOwnership();
            if (owner == nullptr) return true; // cell is free
            int height = cell.getTowerHeight();
            if (height >= rules.maxTowerHeight) continue; // cannot place regardless of rules below
            if (owner == &player && rules.canPlaceOnOwnTowers) return true;
            if ( player.sameTeam(*owner) && rules.canPlaceOnAllyTowers) return true;
            if (!player.sameTeam(*owner) && rules.canPlaceOnEnemyTowers) return true;
        }
    return false;
}

bool GameModel::canTransferAnything(const Player& player) const {
    if (!rules.canTransferToAlly()) return false;
    const std::vector<Checker>& tray = trays.at(player.slot);
    for (const Checker& c : tray) {
        const Player* p = c.getPlayerReference();
        if (isNull(p, "GameModel::canTransferAnything: found Cell's null Player reference")) continue;
        if (p == &player) continue; // skip our own
        if (player.sameTeam(*p) && !p->defeated) return true;
    }
    return false;
}

bool GameModel::canPlaceReserve(const Coord& cd, const Player& ofPlayer, int amount) const {
    if (ofPlayer.isSpectator()) return false;
    if (amount < 1 || amount > rules.maxReservePlaced) return false;
    if (!board.validCoordinate(cd)) return false;
    
    const Player& currentPlayer = getCurrentPlayer();
    if (!ofPlayer.sameTeam(currentPlayer)) return false;
    if (!rules.canPlaceAlliedReserve && ofPlayer != currentPlayer) return false;

    const Cell& cell = board[cd];
    if (!cell.isPlayable()) return false;
    if (cell.isPole() && !rules.canPlaceOnPoles) return false;
    const Player* owner = cell.getOwnership();
    if (!( // can place vs cell owner
        owner == nullptr ||
        (owner == &currentPlayer && rules.canPlaceOnOwnTowers) ||
        ( currentPlayer.sameTeam(*owner) && rules.canPlaceOnAllyTowers) ||
        (!currentPlayer.sameTeam(*owner) && rules.canPlaceOnEnemyTowers) 
    )) return false;
    if (!( // can place vs cell tower height
        rules.canExceedByPlacing ||
        amount <= (rules.maxTowerHeight - cell.getTowerHeight()) 
    )) return false;
    if ( // can place vs own reserve size
        getTraySize(currentPlayer.slot, ofPlayer.slot) < amount
    ) return false;
    return true;
}

bool GameModel::canTransferCheckers(const Player& toPlayer, int amount) const {
    const Player& currentPlayer = getCurrentPlayer();
    if (amount < 1 || amount > rules.allyReserveTransferLimit) return false;
    if (toPlayer == currentPlayer || !toPlayer.sameTeam(currentPlayer)) return false;
    int traySize = getTraySize(currentPlayer.slot, toPlayer.slot);
    return traySize >= amount;
}

int GameModel::getTraySize(PlayerSlot ownedByPlayer, PlayerSlot ofPlayer) const {
    if (trays.find(ownedByPlayer) == trays.end()) return 0;
    const auto& tray = trays.at(ownedByPlayer);
    int result = 0;
    for (const Checker& c : tray) result += c.getPlayerReference()->slot == ofPlayer;
    return result;
}

void GameModel::updateDefeatedPlayers() {
    for (int i = 0; i < players.size(); ++i) {
        Player& p = players[i];
        if (p.defeated) continue; // already defeated
        if (isPlayerDefeated(p)) p.defeated = true;
    }
}

void GameModel::putExcessToTray(const SessionKey& key, const Coord& from) {
    std::vector<Checker>& stack = board.at(from).getCheckers(key);
    if (stack.size() <= rules.maxTowerHeight) return;
    const Player& currentPlayer = getCurrentPlayer();
    auto start = stack.begin();
    auto end = stack.begin() + (stack.size() - rules.maxTowerHeight);
    for (auto iter = start; iter != end; ++iter) { // mark the chunk for display
        iter->putInTrayOf(key, currentPlayer.slot);
    }
    std::vector<Checker>& tray = trays.at(getCurrentPlayer().slot);
    moveAndAppend(stack, tray, start, end);
}

bool GameModel::move(const SessionKey& key, const Coord& from, const Coord& to) {
    int amount = canMove(from, to);
    if (amount == CANNOT_MOVE) return false;
    std::vector<Checker>& vFrom = board.at(from).getCheckers(key);
    std::vector<Checker>& vTo = board.at(to).getCheckers(key);
    moveAndAppend(vFrom, vTo, vFrom.end() - amount, vFrom.end());
    putExcessToTray(key, to);
    return true;
}

bool GameModel::placeReserve(const SessionKey& key, const Coord& cd, const Player& ofPlayer, int amount) {
    if (!canPlaceReserve(cd, ofPlayer, amount)) return false;
    int toPlace = amount;
    const Player& currentPlayer = getCurrentPlayer();
    std::vector<Checker>& tray = trays.at(currentPlayer.slot);
    for (auto iter = tray.begin(); toPlace > 0 && iter != tray.end(); ) {
        if (iter->getPlayerReference() == &ofPlayer) {
            board.place(key, cd, std::move(*iter));
            iter = tray.erase(iter);
            --toPlace;
        } else {
            ++iter;
        }
    }
    putExcessToTray(key, cd);
    return true;
}

bool GameModel::transferCheckers(const SessionKey& key, const Player& toPlayer, int amount) {
    if (!canTransferCheckers(toPlayer, amount)) return false;
    int toPlace = amount;
    const Player& currentPlayer = getCurrentPlayer();
    std::vector<Checker>& tray = trays.at(currentPlayer.slot);
    for (auto iter = tray.begin(); toPlace > 0 && iter != tray.end(); ) {
        if (iter->getPlayerReference() == &toPlayer) {
            trays.at(toPlayer.slot).push_back(std::move(*iter));
            iter = tray.erase(iter);
            --toPlace;
        } else {
            ++iter;
        }
    }
    return true;
}

void GameModel::transferMove(const SessionKey& key) {
    int ppl = players.size();
    if (ppl == 0) return; // gameInProgress won't ever be set to true
    int startIndex = activePlayerIndex;
    do {
        activePlayerIndex = (activePlayerIndex + 1) % ppl;
        const Player& currentPlayer = getCurrentPlayer();
        if (!currentPlayer.isSpectator() && !currentPlayer.defeated) break;
    } while (activePlayerIndex != startIndex);
    gameInProgress = (activePlayerIndex != startIndex);
    if (!gameInProgress) return;

    // additional check if all the remaining players are in the same team
    // game is over when there's only one team representatives left
    TeamSlot firstTeam = TeamSlot::Spectator;
    for (int i = 0; i < ppl; ++i) {
        const Player& p = players[i];
        if (p.isSpectator() || p.defeated) continue;
        if (firstTeam == TeamSlot::Spectator) {
            firstTeam = p.team;
        } else if (firstTeam != p.team) {
            gameInProgress = true;
            return;
        }
    }
    gameInProgress = false;
}

GameModel::GameModel(GameBoard&& _board, std::vector<Player> _players) :
board(std::move(_board)),
players(std::move(_players)),
gameInProgress(false),
activePlayerIndex(players.size() - 1) { // set active index to the last player before the game start
    for (const Player& player : players) {
        if (player.isSpectator()) continue;
        trays.insert({ player.slot, std::vector<Checker>{} });
    }
}

int GameModel::canMove(const Coord& cFrom, const Coord& cTo) const {
    if (!board.validCoordinate(cFrom) || !board.validCoordinate(cTo)) return CANNOT_MOVE;
    if (cFrom == cTo) return CANNOT_MOVE;
    const Cell& from = board[cFrom];
    const Player* owner = from.getOwnership();
    const Player& currentPlayer = getCurrentPlayer();
    if (owner != &currentPlayer) return CANNOT_MOVE;
    const Cell& to = board[cTo];
    int minDistance = from.getTowerHeight();
    if (minDistance == 0) return CANNOT_MOVE;
    if (!from.isPlayable() || !to.isPlayable()) return CANNOT_MOVE;

    bool moveIsPossible = false;
    moveIsPossible |= board.getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Up   , minDistance);
    moveIsPossible |= board.getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Down , minDistance);
    moveIsPossible |= board.getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Left , minDistance);
    moveIsPossible |= board.getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Right, minDistance);
    return moveIsPossible ? minDistance : CANNOT_MOVE;
}

void GameModel::TEMPputCheckerIntoTray(const EditorKey& key, const Player& p, Checker&& c) {
    c.putInTrayOf(key, p.slot);
    trays[p.slot].push_back(std::move(c));
}

void GameModel::start(const SessionKey& key) {
    transferMove(key);
}

bool GameModel::makeTurn(const SessionKey& key, const Turn& turn) {
    if (!gameInProgress) return false;
    bool canMakeTurn = false;
    const Player* player = getPlayerBySlot(turn.assocPlayer);
    switch (turn.type) {
        case TurnType::Move:
            canMakeTurn = move(key, turn.from, turn.to);
            break;
        if (player == nullptr) { canMakeTurn = false; break; }
        
        case TurnType::Place:
            canMakeTurn = placeReserve(key, turn.to, *player, turn.checkerAmount);
            break;
        case TurnType::Transfer:
            canMakeTurn = transferCheckers(key, *player, turn.checkerAmount);
    }
    if (!canMakeTurn) return false;
    updateDefeatedPlayers();
    transferMove(key);
    return true;
}

GameModel::MovePossibility GameModel::getPossibleMovesFor(const Coord& cd) const {
    MovePossibility mp;
    if (!board.validCoordinate(cd)) return mp;
    const Cell& cell = board[cd];
    const Player& currentPlayer = getCurrentPlayer();
    int height = cell.getTowerHeight();
    const Player* owner = cell.getOwnership();
    bool own = false, enemy = false, allied = false;
    if (owner != nullptr) {
        own = owner == &currentPlayer;
        enemy = !owner->sameTeam(currentPlayer);
        allied = (!own && !enemy);
    };
    if (own && !rules.canPlaceOnOwnTowers ||
        enemy && !rules.canPlaceOnEnemyTowers ||
        allied && !rules.canPlaceOnAllyTowers) {
        mp.maxReservePlaced = 0;
    } else {
        mp.maxReservePlaced = rules.canExceedByPlacing ? rules.maxTowerHeight : rules.maxTowerHeight - height;
    }
    if (cell.getOwnership() != &currentPlayer || height == 0) return mp;

    // fill up all possible moves
    for (idxtype i = 0; i < board.sizes.x; ++i)
        for (idxtype j = 0; j < board.sizes.y; ++j) {
            Coord dest(i, j);
            int minDistance = canMove(cd, dest);
            if (minDistance == CANNOT_MOVE) continue;
            mp.canGoTo.push_back({ &board[dest], minDistance });
        }
    return mp;
}

Coord GameModel::locateCheckerOnBoard(const Checker& c) const {
    for (idxtype i = 0; i < board.sizes.x; ++i)
        for (idxtype j = 0; j < board.sizes.y; ++j) {
            Coord cd(i, j);
            const Cell& cell = board[cd];
            const std::vector<Checker>& checkers = cell.getCheckers();
            for (const Checker& checker : checkers)
                if (&checker == &c) // comparing checkers by addresses since they can't be copied
                    return cd;
        }
    return Coord::INVALID_COORD;
}

Coord GameModel::getCellCoord(const Cell& c) const {
    for (idxtype i = 0; i < board.sizes.x; ++i)
        for (idxtype j = 0; j < board.sizes.y; ++j) {
            Coord cd(i, j);
            const Cell& cell = board[cd];
            if (&cell == &c) return cd;
        }
    return Coord::INVALID_COORD;
}

bool GameModel::isSelectableChecker(const Checker& c) const {
    const Player& currentPlayer = getCurrentPlayer();
    if (!c.isOnBoard()) { // is in someone's tray
        if (!c.isInTrayOf(currentPlayer.getSlot())) return false;
        const Player* owner = c.getPlayerReference();
        if (!owner->sameTeam(currentPlayer)) return false; // cannot operate with captured checkers
        if (owner == &currentPlayer) return rules.canPlaceReserve();
        return rules.canPlaceAlliedReserve;
    }
    Coord cd = locateCheckerOnBoard(c);
    return cd != Coord::INVALID_COORD && board[cd].getOwnership() == &currentPlayer;
}