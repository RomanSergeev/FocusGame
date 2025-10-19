#include "BoardEditor.h"
#include "Checker.h"
#include "Constants.h"
#include "GameModel.h"
#include "model/GameBoard.h"
#include "model/Player.h"

#define CC(i, j, p) resultBoard.placeNew(key, {i, j}, Checker(players.at(p)))

GameModel BoardEditor::createBoard8x8Focus() {
    GameBoard resultBoard({8, 8});
    BoardEditor instance(resultBoard);
    EditorKey key = instance.key;
    std::vector<Player> players;
    // resultBoard.markCell(key, {2,3}, true, false);
    players.reserve(2);
    players.emplace_back(key, PlayerSlot::Player1, TeamSlot::Team1, PlayerType::HUMAN_LOCAL);
    players.emplace_back(key, PlayerSlot::Player2, TeamSlot::Team2, PlayerType::HUMAN_LOCAL);
    resultBoard.setupCellCoordinates(key);

    resultBoard.markUnplayableByCondition(key, [](unsigned int sizeX, unsigned int sizeY, int i, int j) {
        float distX = sizeX / 2.0 - 0.5 - i;
        float distY = sizeY / 2.0 - 0.5 - j;
        float distance = distX*distX + distY*distY;
        float removeDist = 2 * (sizeX / 2.0 - 1.5) * (sizeX / 2.0 - 1.5) + 2.5;
        return distance >= removeDist;
    });

    for (idxtype i = 1; i < 7; ++i)
        for (idxtype j = 1; j < 7; ++j) {
            int playerIndex = !((((j+1)>>1)+i)&1);
            CC(j, i, playerIndex); // create checker
        }

    GameModel result(std::move(resultBoard), std::move(players));
    /*const Player& p = result.TEMPgetPlayer(0);
    const Player& q = result.TEMPgetPlayer(1);
    result.TEMPputCheckerIntoTray(key, q, Checker(p)); // should not be displayed at game start*/
    return result;
}

GameModel BoardEditor::createBoard8x8Chess() {
    GameBoard resultBoard({8, 8});
    BoardEditor instance(resultBoard);
    EditorKey key = instance.key;
    std::vector<Player> players;
    players.reserve(2);
    players.emplace_back(key, PlayerSlot::Player1, TeamSlot::Team1, PlayerType::HUMAN_LOCAL);
    players.emplace_back(key, PlayerSlot::Player2, TeamSlot::Team2, PlayerType::HUMAN_LOCAL);
    resultBoard.setupCellCoordinates(key);

    for (idxtype i = 0; i < 2; ++i)
        for (idxtype j = 0; j < 8; ++j)
            CC(j, i, 0);

    for (idxtype i = 6; i < 8; ++i)
        for (idxtype j = 0; j < 8; ++j)
            CC(j, i, 1);

    GameModel result(std::move(resultBoard), std::move(players));
    return result;
}

GameModel BoardEditor::createBoard2x2Debug() {
    GameBoard resultBoard({6, 2});
    BoardEditor instance(resultBoard);
    EditorKey key = instance.key;
    std::vector<Player> players;
    players.reserve(2);
    players.emplace_back(key, PlayerSlot::Player1, TeamSlot::Team1, PlayerType::HUMAN_LOCAL);
    players.emplace_back(key, PlayerSlot::Player2, TeamSlot::Team2, PlayerType::HUMAN_LOCAL);
    resultBoard.setupCellCoordinates(key);

    CC(1, 1, 0);
    CC(0, 0, 1);
    CC(0, 1, 1);
    CC(0, 1, 1);
    CC(0, 1, 1);
    CC(0, 1, 1);
    CC(0, 1, 1);
    CC(1, 0, 0);
    CC(1, 0, 0);
    CC(1, 0, 0);
    CC(1, 0, 0);
    CC(1, 0, 0);

    GameModel result(std::move(resultBoard), std::move(players));
    const Player& p = result.TEMPgetPlayer(0);
    const Player& q = result.TEMPgetPlayer(1);
    result.TEMPputCheckerIntoTray(key, p, Checker(p)); // should not be displayed at game start
    result.TEMPputCheckerIntoTray(key, p, Checker(p)); // should not be displayed at game start
    result.TEMPputCheckerIntoTray(key, p, Checker(p)); // should not be displayed at game start
    result.TEMPputCheckerIntoTray(key, p, Checker(p)); // should not be displayed at game start
    result.TEMPputCheckerIntoTray(key, p, Checker(p)); // should not be displayed at game start
    return result;
}