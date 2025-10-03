#pragma once
#include "model/GameModel.h"
#include "view/GameView.h"

class GameSession {
    GameModel model;
    GameView view;
    GameView::SelectedView selectedShape;
    SessionKey key;
public:
    GameSession(GameModel&& model, GameView::BoardShapeType type) : model(std::move(model)), view(this->model, type) {}
    void selectShapeFromCameraRay(const Ray& ray);
    void drawBoard(const Shader& shader, float currentTime);
};