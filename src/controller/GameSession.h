#pragma once
#include <unordered_map>
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "view/GameView.h"

class GameSession {
    using SV = GameView::SelectableView;
    enum class HighlightLogic { // which cells to highlight as available for landing your selected checkers
        None, // you just perform a move if rules allow you to, no helper highlights
        PossibleOnHover, // if it's possible to make a move on this cell - it will glow green
        AllOnHover, // PossibleOnHover + if it's not possible - a cell will glow red
        AllPossible // for a current selection, always highlight in green all cells this selection can go to
    };
    struct SelectionData {
        enum class SelectedEntity { // something that is selected and upon what move decision is made
            None,
            Tower,
            Reserve
        };
        SelectionData() { drop(); }
        SelectionData(const Coord& cd) { set(cd); }
        SelectionData(PlayerSlot whoseReserve) { set(whoseReserve); }

        SelectedEntity getSelectedEntity() const { return whatsSelected; }
        Coord getCoordinate() const { return towerCoord; }
        PlayerSlot getAssociatedPlayer() const { return whoseReserveSelected; }
        const std::vector<SV>& getCheckers() const { return selectedCheckers; }
        bool isEmpty() const { return whatsSelected == SelectedEntity::None; }

        void set(const Coord& cd);
        void set(PlayerSlot whoseReserve);
        void drop();
        void toggle(SV sv); // for reserve checkers: add or remove by checker selection
        void add(SV sv);
        void selectTowerTop(int size);
    private:
        Coord towerCoord;
        PlayerSlot whoseReserveSelected;
        std::vector<SV> selectedCheckers; // only CheckerView is allowed
        SelectedEntity whatsSelected = SelectedEntity::None; // a bit of data duplication, but it's handled
    };

    GameModel model;
    GameView view;
    SV hoveredShape;
    SV pressedShape; // its only purpose is to check that mousedown shape == mouseup shape
    std::vector<SV> highlightedShapes = {};
    std::unordered_map<Coord, GameModel::MovePossibility> allPossibleMoves;
    SessionKey key;
    HighlightLogic howToHighlight = HighlightLogic::PossibleOnHover;
    SelectionData storedSelection;
    bool selectionLocked = false; // when it's not our turn - all selection is cleared, and none can be added

    void clearPossibleMoves();
    void calculatePossibleMoves();
    void onHover(const SV& sv);
    void onClick(); // no argument - assuming hoveredShape is clicked (ruled out before the call)
    void clearAllSelection();
    void lockSelection();
    void initSelection(); // fills up storedSelection based on the current checker clicked, being the first such checker
    bool performTurn(const GameModel::Turn& turn);
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type);
    void hoverShapeByCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
    void handleMouseDown(const Ray& ray);
    void handleMouseUp(const Ray& ray);
};