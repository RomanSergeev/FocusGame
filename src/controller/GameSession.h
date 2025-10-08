#pragma once
#include "model/GameModel.h"
#include "view/GameView.h"
#include <unordered_map>

class GameSession {
    enum class SelectedEntity { // something that is selected and upon what move decision is made
        None,
        Tower,
        OwnReserve,
        AlliedReserve
    };
    enum class HighlightLogic { // which cells to highlight as available for landing your selected checkers
        None, // you just perform a move if rules allow you to, no helper highlights
        PossibleOnHover, // if it's possible to make a move on this cell - it will glow green
        AllOnHover, // PossibleOnHover + if it's not possible - a cell will glow red
        AllPossible // for a current selection, always highlight in green all cells this selection can go to
    };

    GameModel model;
    GameView view;
    GameView::SelectableView hoveredShape;
    GameView::SelectableView shapeForSelection;
    std::vector<GameView::SelectableView> selectedShapes = {};
    std::vector<GameView::SelectableView> highlightedShapes = {}; // might only be used with HighlightLogic::AllPossible option
    std::unordered_map<Coord, GameModel::MovePossibility> allPossibleMoves;
    SessionKey key;
    SelectedEntity nowSelected = SelectedEntity::None;
    HighlightLogic howToHighlight = HighlightLogic::PossibleOnHover;

    void clearPossibleMoves();
    void calculatePossibleMoves();
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type);
    void hoverShapeByCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
    void handleMouseDown(const Ray& ray);
    void handleMouseUp(const Ray& ray);
};