#include "GameSession.h"
#include "utility/Utils.h"
#include "view/GameView.h"

void GameSession::hoverShapeByCameraRay(const Ray& ray) {
    GameView::SelectedView newSelected = view.getHoveredShape(key, ray);
    bool wasSelected = vectorContains(selectedShapes, hoveredShape);
    hoveredShape.select(wasSelected ? SelectionType::Selected : SelectionType::NoSelection);
    hoveredShape = newSelected;
    hoveredShape.select(SelectionType::ForSelection);
}

void GameSession::drawBoard(const Shader& shader, float currentTime) {
    PlayerSlot perspective = model.getCurrentPlayer().getSlot();
    view.draw(perspective, shader, currentTime);
}

void GameSession::handleMouseDown(const Ray& ray) {
    shapeForSelection = hoveredShape; // view.getHoveredShape(key, ray); // is more robust but expensive
}

void GameSession::handleMouseUp(const Ray& ray) {
    GameView::SelectedView copy = shapeForSelection;
    shapeForSelection.drop();
    if (hoveredShape != copy || // we should release mouse hovering the same shape we pressed on
        !hoveredShape.isCheckerView()) return; // and it should be a checker (for now)
    auto iter = std::find(selectedShapes.begin(), selectedShapes.end(), hoveredShape);
    if (iter == selectedShapes.end()) {
        selectedShapes.push_back(hoveredShape);
        hoveredShape.select(SelectionType::Selected);
    } else {
        selectedShapes.erase(iter);
        hoveredShape.select(SelectionType::ForSelection);
    }
}