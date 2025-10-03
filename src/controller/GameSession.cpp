#include "GameSession.h"

void GameSession::selectShapeFromCameraRay(const Ray& ray) {
    GameView::SelectedView newSelected = view.TEMPselectShapeByIntersection(key, ray);
    selectedShape.select(SelectionType::NoSelection);
    selectedShape = newSelected;
    selectedShape.select(SelectionType::Selected);
}

void GameSession::drawBoard(const Shader& shader, float currentTime) {
    PlayerSlot perspective = model.getCurrentPlayer().getSlot();
    view.draw(perspective, shader, currentTime);
}