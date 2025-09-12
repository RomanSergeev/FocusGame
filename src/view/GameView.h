#pragma once
#include <memory>
#include <unordered_map>
#include "model/GameModel.h"
#include "shapes/OpenGLShape.h"

class GameView {
    enum class BoardShapeType {
        Flat,
        Toroid,
        Sphere
    };

    struct CellView {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 anchorPoint;
        glm::vec3 upVector;

        CellView() = default;
        CellView(const CellView& dc) = delete;
        CellView& operator = (const CellView& dc) = delete;
        CellView(CellView&& dc) noexcept = default;
        CellView& operator = (CellView&& dc) noexcept = default;
    };

    struct CheckerView {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 position;
        glm::vec3 targetPosition;
        float animationProgress = 0.0f;
        const Checker* checkerRef;

        CheckerView() = default;
        CheckerView(const CheckerView& dc) = delete;
        CheckerView& operator = (const CheckerView& dc) = delete;
        CheckerView(CheckerView&& dc) noexcept = default;
        CheckerView& operator = (CheckerView&& dc) noexcept = default;
    };

    static constexpr float FB_CELL_WIDTH = 2; // flat board
    static constexpr float FB_CELL_HEIGHT_J = .6;
    static constexpr float FB_CELL_HEIGHT_NJ = 2;
    static constexpr float CHECKER_HALF_WIDTH = .8;
    static constexpr float CHECKER_HALF_HEIGHT = .1;
    static const float FB_CELL_HEIGHT[2];
    static const float FB_CELL_ORIGIN[2];
    static const float FB_CELL_ANCHOR[2];
    
    const GameModel& model;
    std::vector<std::vector<CellView>> displayedBoard;
    std::vector<CheckerView> displayedCheckers;
    std::unordered_map<PlayerSlot, Color> displayedPlayerColors;
    BoardShapeType type;

    void fillDisplayedBoard();
public:
    GameView(const GameModel& gm);
    GameView(const GameView& gv) = delete;
    GameView& operator = (const GameView& gv) = delete;

    void draw(const Shader& shader, float currentTime);
    void TEMPdeselectAll();
    void TEMPselectDistinctCell(int i, int j);
    void TEMPselectDistinctChecker(const Checker& c);
    void TEMPstageCheckerSelection(const Checker& c);
    float TEMPselectShapeByIntersection(const Ray& ray);
    void updatePlayerColors(const SessionKey& key, const std::unordered_map<PlayerSlot, Color>& colors) { displayedPlayerColors = colors; }
};