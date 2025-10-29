#pragma once
#include <unordered_map>
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "view/GameView.h"

class GameSession {
    using SV = GameView::SelectableView;
    enum class HighlightMethod { // which cells to highlight as available for landing your selected checkers
        Minimal, // everything glows yellow if it can be added to selection, or a move can be performed
        PossibleOnHover, // if it's possible to make a move on this cell - it will glow green
        AllOnHover, // PossibleOnHover + if it's not possible - a cell will glow red
        AllPossible // for a current selection, always highlight in green all cells this selection can go to
    };
    enum class Action {
        Impossible,
        Select,
        Move,
        Place
    };
    struct AssumedAction {
        static const AssumedAction NONE;
        Action action;
        int checkersUsed;
        AssumedAction() : action(Action::Impossible), checkersUsed(0) {}
        AssumedAction(Action action) : action(action), checkersUsed(0) {}
        AssumedAction(Action action, int checkersUsed) : action(action), checkersUsed(checkersUsed) {}
        AssumedAction(const AssumedAction& aa) = default;

        AssumedAction& operator = (const AssumedAction& aa) = default;
        AssumedAction& operator = (Action _action) { action = _action; checkersUsed = 0; return *this; }

        void clear() { action = Action::Impossible; checkersUsed = 0; }
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
        int getSize() const { return selectedCheckers.size(); }
        bool isEmpty() const { return whatsSelected == SelectedEntity::None; }

        void set(const Coord& cd);
        void set(PlayerSlot whoseReserve);
        void drop();
        void toggle(SV sv); // for reserve checkers: add or remove by checker selection
        void add(SV sv);
        void selectTowerTop(int size);
        void selectTowerAll();
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
    std::vector<SV> alwaysHighlightedShapes = {};
    std::unordered_map<Coord, GameModel::MovePossibility> allPossibleMoves;
    SessionKey key;
    HighlightMethod highlightMethod = HighlightMethod::AllPossible;
    SelectionData storedSelection;
    AssumedAction assumedAction = AssumedAction::NONE;
    bool selectionLocked = false; // when it's not our turn - all selection is cleared, and none can be added

    void clearPossibleMoves();
    void calculatePossibleMoves();
    int getCachedMoveDistance(const Coord& from, const Coord& to) const;
    HighlightState getRetainedState() const;
    AssumedAction assumeAction() const;
    void onHover(const SV& sv);
    void onClick(); // no argument - assuming hoveredShape is clicked (ruled out before the call)
    void clearAllSelection();
    void lockSelection();
    void addAlwaysHighlightedCell(SV sv);
    void clearAlwaysHighlightedCells();
    void updateAlwaysHighlightedShapes();
    void initSelection(); // fills up storedSelection based on the current checker clicked, being the first such checker
    bool performTurn(const GameModel::Turn& turn);
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type);
    void hoverShapeByCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
    void handleMouseDown(const Ray& ray);
    void handleMouseUp(const Ray& ray);
};