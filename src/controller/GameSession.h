#pragma once
#include "model/GameModel.h"
#include "view/GameView.h"

class GameSession {
    GameModel model;
    GameView view;
    GameView::SelectedView hoveredShape;
    GameView::SelectedView shapeForSelection;
    std::vector<GameView::SelectedView> selectedShapes = {};
    SessionKey key;
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type) : model(std::move(model)), view(this->model, type) {}
    void hoverShapeByCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
    void handleMouseDown(const Ray& ray);
    void handleMouseUp(const Ray& ray);
};