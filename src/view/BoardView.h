#pragma once
#include <memory>
#include "model/GameBoard.h"
#include "shapes/OpenGLShape.h"

class BoardView {
    enum class BoardShapeType {
        Flat,
        Toroid,
        Sphere
    };

    struct DisplayedCell {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 anchorPoint;
        glm::vec3 upVector;

        DisplayedCell() = default;
        DisplayedCell(const DisplayedCell& dc) = delete;
        DisplayedCell& operator = (const DisplayedCell& dc) = delete;
        DisplayedCell(DisplayedCell&& dc) noexcept = default;
        DisplayedCell& operator = (DisplayedCell&& dc) noexcept = default;
    };

    struct DisplayedChecker {
        std::unique_ptr<OpenGLShape> shape;
        glm::vec3 position;
        glm::vec3 targetPosition;
        float animationProgress = 0.0f;
        const Checker* checkerRef;

        DisplayedChecker() = default;
        DisplayedChecker(const DisplayedChecker& dc) = delete;
        DisplayedChecker& operator = (const DisplayedChecker& dc) = delete;
        DisplayedChecker(DisplayedChecker&& dc) noexcept = default;
        DisplayedChecker& operator = (DisplayedChecker&& dc) noexcept = default;
    };

    static constexpr float CUBIC_CELL_WIDTH = 2;
    static constexpr float CUBIC_CELL_DEPTH = .6;
    static constexpr float CHECKER_HALF_WIDTH = .8;
    static constexpr float CHECKER_HALF_HEIGHT = .1;
    
    GameBoard& board;
    std::vector<std::vector<DisplayedCell>> displayedBoard;
    std::vector<DisplayedChecker> displayedCheckers;
    BoardShapeType type;

    void fillDisplayedBoard();
public:
    BoardView(GameBoard& gb);
    BoardView(const BoardView& bv) = delete;
    BoardView& operator = (const BoardView& bv) = delete;

    void draw(const Shader& shader, float currentTime);
};