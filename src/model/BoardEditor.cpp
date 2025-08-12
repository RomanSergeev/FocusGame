#include "BoardEditor.h"
#include "Checker.h"
#include "Constants.h"
#include "GameBoard.h"

GameBoard BoardEditor::createBoard8x8Focus() {
    GameBoard result(8, 8);
    BoardEditor instance(result);
    EditorKey key = instance.key;

    result.markUnplayableByCondition(key, [](unsigned int sizeX, unsigned int sizeY, int i, int j) {
        float distX = sizeX / 2.0 - 0.5 - i;
        float distY = sizeY / 2.0 - 0.5 - j;
        float distance = distX*distX + distY*distY;
        float removeDist = 2 * (sizeX / 2.0 - 1.5) * (sizeX / 2.0 - 1.5) + 2.5;
        return distance >= removeDist;
    });

    for (int i = 1; i < 7; ++i)
        for (int j = 1; j < 7; ++j) {
            PlayerPlaceholderID playerId = ((((j+1)>>1)+i)&1) ? PlayerPlaceholderID::Player1 : PlayerPlaceholderID::Player2;
            result.place(key, i, j, Checker(playerId));
            //result.place(key, i, j, Checker(playerId));
        }

    return result;
}

GameBoard BoardEditor::createBoard8x8Chess() {
    GameBoard result(8, 8);
    BoardEditor instance(result);
    EditorKey key = instance.key;

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 8; ++j)
            result.place(key, i, j, Checker(PlayerPlaceholderID::Player1));

    for (int i = 6; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            result.place(key, i, j, Checker(PlayerPlaceholderID::Player2));

    return result;
}