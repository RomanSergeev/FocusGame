#include "GameSession.h"
#include "Constants.h"
#include "model/GameModel.h"
#include "utility/Utils.h"
#include "view/GameView.h"

void GameSession::clearPossibleMoves() {
    for (auto iter : allPossibleMoves)
        iter.second.clear();
}

void GameSession::calculatePossibleMoves() {
    clearPossibleMoves();
    // if current player is remote and isn't us - return
    const Player& currentPlayer = model.getCurrentPlayer();
    for (idxtype i = 0; i < model.getRows(); ++i)
        for (idxtype j = 0; j < model.getColumns(); ++j) {
            Coord cd(i, j);
            allPossibleMoves[cd] = model.getPossibleMovesFor(cd);
        }
}

void GameSession::onHover(const SV& sv) {
    bool wasSelected = vectorContains(selectedShapes, hoveredShape);
    hoveredShape.select(wasSelected ? SelectionType::Selected : SelectionType::NoSelection);
    hoveredShape = sv;
    hoveredShape.select(SelectionType::ForSelection);
}

void GameSession::onClick() { // called only when it's our turn
    if (hoveredShape.isEmpty()) return;
    // SelectionData clicked = 
    if (hoveredShape.isCellView()) { // clicking on a cell - something should happen only when we have stuff selected

    } else if (hoveredShape.isCheckerView()) { // clicking on a checker - first define where it is

    }
    auto iter = std::find(selectedShapes.begin(), selectedShapes.end(), hoveredShape);
    if (iter == selectedShapes.end()) {
        selectedShapes.push_back(hoveredShape);
        hoveredShape.select(SelectionType::Selected);
    } else {
        selectedShapes.erase(iter);
        hoveredShape.select(SelectionType::ForSelection);
    }
}

void GameSession::lockSelection() {
    selectedShapes.clear();
    highlightedShapes.clear();
    pressedShape.drop(); // should not be possible to set after the turn, but just in case
    selectionLocked = true;
}

GameSession::GameSession(GameModel&& _model, GameView::BoardShapeType type) :
model(std::move(_model)),
view(model, type) {
    for (idxtype i = 0; i < model.getRows(); ++i)
        for (idxtype j = 0; j < model.getColumns(); ++j) {
            allPossibleMoves.try_emplace({i, j}); // create default entries for each coordinate
        }
    model.start(key);
    view.updateOnCurrentPlayerChange(model.getCurrentPlayer().getSlot());
}

void GameSession::hoverShapeByCameraRay(const Ray& ray) {
    if (selectionLocked) return;
    SV newSelected = view.getHoveredShape(key, ray);
    onHover(newSelected);
}

void GameSession::drawBoard(const Shader& shader, float currentTime) {
    PlayerSlot perspective = model.getCurrentPlayer().getSlot();
    view.draw(perspective, shader, currentTime);
}

void GameSession::handleMouseDown(const Ray& ray) {
    if (selectionLocked) return;
    pressedShape = hoveredShape; // view.getHoveredShape(key, ray); // is more robust but expensive
}

void GameSession::handleMouseUp(const Ray& ray) {
    if (selectionLocked) return;
    SV copy = pressedShape;
    pressedShape.drop();
    if (hoveredShape != copy) return; // we should release mouse hovering the same shape we pressed on
    onClick();
}