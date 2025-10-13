#pragma once
#include <unordered_map>
#include "view/GameView.h"

class GameSession {
    using SV = GameView::SelectableView;
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
    struct SelectionData {
        SelectionData() { clear(); }
        SelectionData(const Coord& cd) : towerCoord(cd) {}
        SelectionData(PlayerSlot whoseReserve) : whoseReserveSelected(whoseReserve) {}
        bool isEmpty() const { return towerCoord == Coord::INVALID_COORD && whoseReserveSelected == PlayerSlot::Spectator; }
        bool isBoardSelection() const { return towerCoord != Coord::INVALID_COORD; }
        bool isTraySelection() const { return whoseReserveSelected != PlayerSlot::Spectator; }
        void clear() { towerCoord = Coord::INVALID_COORD; whoseReserveSelected = PlayerSlot::Spectator; selectedCheckers.clear(); }
        private:
        Coord towerCoord;
        PlayerSlot whoseReserveSelected;
        std::vector<SV> selectedCheckers; // only CheckerView is allowed
    };

    GameModel model;
    GameView view;
    SV hoveredShape;
    SV pressedShape; // its only purpose is to check that mousedown shape == mouseup shape
    std::vector<SV> selectedShapes = {};
    std::vector<SV> highlightedShapes = {};
    std::unordered_map<Coord, GameModel::MovePossibility> allPossibleMoves;
    SessionKey key;
    SelectedEntity nowSelected = SelectedEntity::None;
    HighlightLogic howToHighlight = HighlightLogic::PossibleOnHover;
    SelectionData currentSelection;
    bool selectionLocked = false; // when it's not our turn - all selection is cleared, and none can be added

    void clearPossibleMoves();
    void calculatePossibleMoves();
    void onHover(const SV& sv);
    void onClick(); // no argument - assuming hoveredShape is clicked (ruled out before the call)
    void lockSelection();
    SelectionData locateChecker(const Checker& c) const;
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type);
    void hoverShapeByCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
    void handleMouseDown(const Ray& ray);
    void handleMouseUp(const Ray& ray);
};