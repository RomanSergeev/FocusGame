#include "GameModel.h"
#include "Cell.h"
#include "Constants.h"
#include "utility/Utils.h"

/*
bool isPlayerDefeated(PlayerSlot player) const;
bool canMove(int iFrom, int jFrom, int iTo, int jTo) const;
bool move(const SessionKey& key, int iFrom, int jFrom, int iTo, int jTo);
*/

void GameModel::fillAlliances() {
    for (const auto& player : players) {
        if (!player.isActive()) continue;
        alliances[player.team].push_back(player.slot);
    }
}

void GameModel::updateDefeatedPlayers() {
    for (int i = 0; i < players.size(); ++i) {
        Player& p = players[i];
        if (p.defeated) continue; // already defeated
        if (isPlayerDefeated(p)) p.defeated = true;
    }
}

bool GameModel::isPlayerDefeated(const Player& player) const {
    if (!player.isActive()) return false; // skip spectator

    for (const auto& pair : trays) {
        PlayerSlot whose = pair.first;
        if (whose == player.slot) return false;//false is TEMP -> (!canPlaceAnywhere() && );
        if (!vectorContains(alliances.at(player.team), whose)) continue;
        //TODO continue
    }

    idxtype rows = board.getRows();
    idxtype cols = board.getColumns();
    for (idxtype i = 0; i < rows; ++i)
        for (idxtype j = 0; j < cols; ++j) {
            
        }
}

bool GameModel::canPlaceReserve(const Coord& cd, const Player& fromPlayer, int amount) const {
    if (fromPlayer.slot == PlayerSlot::Spectator) return false;
    if (amount < 1 || amount > rules.maxReservePlaced) return false;
    if (!board.validCoordinate(cd)) return false;
    
    const Player& currentPlayer = players.at(activePlayerIndex);
    if (fromPlayer.team != currentPlayer.team) return false;
    if (!rules.canPlaceAlliedReserve && fromPlayer.slot != currentPlayer.slot) return false;

    const Cell& cell = board[cd];
    if (!cell.isPlayable()) return false;
    if (!rules.canPlaceOnPoles && cell.isPole()) return false;
    const Player* owner = cell.getOwnership();
    if (!( // can place vs cell owner
        owner == nullptr ||
        (owner == &currentPlayer && rules.canPlaceOnOwnTowers) ||
        (owner->team == currentPlayer.team && rules.canPlaceOnAllyTowers) ||
        (owner->team != currentPlayer.team && rules.canPlaceOnEnemyTowers) 
    )) return false;
    if (!( // can place vs cell tower height
        rules.canExceedByPlacing ||
        amount <= (rules.maxTowerHeight - cell.getTowerHeight()) 
    )) return false;
    if ( // can place vs own reserve size
        getTraySize(currentPlayer.slot, fromPlayer.slot) < amount
    ) return false;
    return true;
}

bool GameModel::placeReserve(const SessionKey& key, const Coord& cd, const Player& fromPlayer, int amount) {
    if (!canPlaceReserve(cd, fromPlayer, amount)) return false;
    int toPlace = amount;
    const Player& currentPlayer = players.at(activePlayerIndex);
    std::vector<Checker>& tray = trays.at(currentPlayer.slot);
    for (auto iter = tray.begin(); toPlace > 0 && iter != tray.end(); ) {
        if (iter->getPlayerReference()->slot == fromPlayer.slot) {
            board.place(key, cd, std::move(*iter));
            iter = tray.erase(iter);
            --toPlace;
        } else {
            ++iter;
        }
    }
    return true;
}

int GameModel::canMove(const Coord& cFrom, const Coord& cTo) const {
    if (!board.validCoordinate(cFrom) || !board.validCoordinate(cTo)) return CANNOT_MOVE;
    if (cFrom == cTo) return CANNOT_MOVE;
    const Cell& from = board[cFrom];
    const Player* owner = from.getOwnership();
    const Player& currentPlayer = players.at(activePlayerIndex);
    if (owner != &currentPlayer) return CANNOT_MOVE;
    const Cell& to = board[cTo];
    int count = from.getTowerHeight();
    if (count == 0) return CANNOT_MOVE;
    if (!from.isPlayable() || !to.isPlayable()) return CANNOT_MOVE;
    int smallestJumpDistance = OVERLIMIT_SIZE, distance = OVERLIMIT_SIZE;
    JumpDirection direction;

    distance = getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Up   , count);
    if (distance < smallestJumpDistance) { smallestJumpDistance = distance; direction = JumpDirection::Up   ; }
    distance = getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Down , count);
    if (distance < smallestJumpDistance) { smallestJumpDistance = distance; direction = JumpDirection::Down ; }
    distance = getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Left , count);
    if (distance < smallestJumpDistance) { smallestJumpDistance = distance; direction = JumpDirection::Left ; }
    distance = getCappedJumpDistanceInDirection(cFrom, cTo, JumpDirection::Right, count);
    if (distance < smallestJumpDistance) { smallestJumpDistance = distance; direction = JumpDirection::Right; }
    if (distance == OVERLIMIT_SIZE) return CANNOT_MOVE;
    return distance;
}

bool GameModel::canTransferCheckers(const Player& toPlayer, int amount) const {
    const Player& activePlayer = players.at(activePlayerIndex);
    if (amount < 1 || amount > rules.allyReserveTransferLimit) return false;
    if (toPlayer.slot == activePlayer.slot || toPlayer.team != activePlayer.team) return false;
    int traySize = getTraySize(activePlayer.slot, toPlayer.slot);
    return traySize >= amount;
}

int GameModel::getTraySize(PlayerSlot ownedByPlayer, PlayerSlot ofPlayer) const {
    if (trays.find(ownedByPlayer) == trays.end()) return 0;
    const auto& tray = trays.at(ownedByPlayer);
    int result = 0;
    for (const Checker& c : tray) result += c.getPlayerReference()->slot == ofPlayer;
    return result;
}

int GameModel::getCappedJumpDistanceInDirection(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int maxDistance) const {
    /*auto nextCell = [&](const Coord& c) -> Coord {
        // returns Coord
    };*/
    
    // from is owned by currentPlayer and has checkers on it; from and to are playable; indices are valid and different
    if (!board[cFrom].isPole()) return testMovementInDirectionWithTether(cFrom, cTo, jd, maxDistance, cFrom.y);
    if (isHorizontal(jd)) return OVERLIMIT_SIZE; // poles can only be horizontal - thus, horizontal movement is forbidden
    for (idxtype j = 0; j < board.sizes.y; ++j) {
        int distance = testMovementInDirectionWithTether(cFrom, cTo, jd, maxDistance, j);
        if (distance < OVERLIMIT_SIZE) return distance;
    }
    // we've scanned through all the columns and couldn't reach the destination
    return OVERLIMIT_SIZE;
}

int GameModel::testMovementInDirectionWithTether(const Coord& cFrom, const Coord& cTo, JumpDirection jd, int maxDistance, idxtype tether) const {
    Coord c = cFrom;
    int totalDistance = 0;
    while (totalDistance < maxDistance) {
        bool canAdvance = board.nextCoordinate(c, jd, tether); // coord changes (tries to advance)
        if (!canAdvance) return OVERLIMIT_SIZE;
        ++totalDistance;
        if (c == cTo) return totalDistance; // if we've reached the destination
        if (!board[c].isJumpableOver()) return OVERLIMIT_SIZE; // cell to travel through is not jumpable over
        if (board[c].isPole()) jd = reverseDirection(jd);
    }
    return OVERLIMIT_SIZE;
}

/*bool GameModel::hasJumpableLineBetween(idxtype iFrom, idxtype iTo, idxtype idx, bool vertically) const {
    if (iFrom == iTo) return true;
    bool asc = (iFrom < iTo);
    int d = asc ? 1 : -1;
    for (idxtype i = iFrom + d; i != iTo; i += d) {
        if ( vertically && board.loopedVertically  ) {
            if ( asc && i == board.rows   ) i = 0;
            if (!asc && i == -1           ) i = board.rows - 1;
        }
        if (!vertically && board.loopedHorizontally) {
            if ( asc && i == board.columns) i = 0;
            if (!asc && i == -1           ) i = board.columns - 1;
        }
        idxtype I = vertically ? i : idx, // vertically -> i is iterated
                J = vertically ? idx : i; // horizontally -> j is iterated
        if (vertically && board.board[i][0].isPole()) J = 0; // 
        const Cell& cell = board.board[I][J];
        if (vertically && cell.isPole() && !rules.canJumpThroughPoles) return false;
        if (!cell.isJumpableOver()) return false;
    }
    return true;
}*/

bool GameModel::transferCheckers(const SessionKey& key, const Player& toPlayer, int amount) {
    if (!canTransferCheckers(toPlayer, amount)) return false;
    int toPlace = amount;
    const Player& currentPlayer = players.at(activePlayerIndex);
    std::vector<Checker>& tray = trays.at(currentPlayer.slot);
    for (auto iter = tray.begin(); toPlace > 0 && iter != tray.end(); ) {
        if (iter->getPlayerReference()->slot == toPlayer.slot) {
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
    // important: game should not be over if transferMove is called
    do {
        ++activePlayerIndex;
        if (activePlayerIndex == players.size())
            activePlayerIndex = 0;
    } while (players[activePlayerIndex].isDefeated() || !players[activePlayerIndex].isActive());
}