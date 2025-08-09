#include "BoardView.h"
#include "Constants.h"
#include "model/GameBoard.h"
#include <memory>
#include "shapes/Cuboid.h"
#include "shapes/Cylinder.h"

void BoardView::fillDisplayedBoard() {
    int rows = board.getRows(),
        cols = board.getColumns();
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            const Cell& cell = board.getCellAt(i, j);
            bool playable = cell.isPlayable();
            if (!playable) continue;
            bool jumpable = cell.isJumpableOver();
            
            // board
            DisplayedCell& dcell = displayedBoard[i][j];
            dcell.upVector = axisToVec3(Axis::Z);
            float x = (i - rows / 2.0 + 0.5) * CUBIC_CELL_WIDTH;
            float y = (j - cols / 2.0 + 0.5) * CUBIC_CELL_WIDTH;
            dcell.anchorPoint = glm::vec3(x, y, jumpable ? CUBIC_CELL_DEPTH / 2 : CUBIC_CELL_WIDTH / 2);
            std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cuboid>(CUBIC_CELL_WIDTH, CUBIC_CELL_WIDTH, jumpable ? CUBIC_CELL_DEPTH : CUBIC_CELL_WIDTH);
            bool even = (i + j) & 1;
            float rgb = even ? 0.2 : 0.9;
            newShape->setColor(rgb, rgb, rgb);
            newShape->translate(x, y, 0);
            dcell.shape = std::move(newShape);

            // checkers
            const auto& checkers = cell.getCheckers();
            for (int k = 0; k < checkers.size(); ++k) {
                const Checker& checker = checkers.at(k);
                DisplayedChecker dchecker;
                dchecker.checkerRef = &checker;
                dchecker.position = glm::vec3(dcell.anchorPoint + dcell.upVector * (CHECKER_HALF_HEIGHT * (2*k + 1)));
                std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cylinder>(CHECKER_HALF_WIDTH, CHECKER_HALF_WIDTH, CHECKER_HALF_HEIGHT, 32);
                newShape->setColor(getDefaultColor(getPlayerOrdinal(checker.getPlaceholderID())).toVec3());
                newShape->translate(dchecker.position);
                dchecker.shape = std::move(newShape);
                displayedCheckers.push_back(std::move(dchecker));
            }
        }
}

BoardView::BoardView(GameBoard& gb) : board(gb), type(BoardShapeType::Flat) {
    displayedBoard.resize(gb.getRows()); // TODO fix the error here
    for (auto& row : displayedBoard) {
        row.resize(gb.getColumns());
    }
    std::cout << "BoardView size: " << displayedBoard.size() 
          << " x " << displayedBoard[0].size() << "\n";
    fillDisplayedBoard();
}

void BoardView::draw(const Shader& shader, float currentTime) {
    for (const auto& row : displayedBoard) {
        for (const auto& dcell : row) {
            if (dcell.shape == nullptr) continue;
            dcell.shape->setUniforms(shader, currentTime);
            dcell.shape->draw();
        }
    }
    for (const auto& dchecker : displayedCheckers) {
        if (dchecker.shape == nullptr) continue;
        dchecker.shape->setUniforms(shader, currentTime);
        dchecker.shape->draw();
    }
}