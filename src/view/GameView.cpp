#include "GameView.h"
#include "shapes/Cuboid.h"
#include "shapes/Cylinder.h"

void checkShapeIntersection(const Ray& ray, OpenGLShape* shape, OpenGLShape* &selectedShape, float& minDist) {
    if (shape == nullptr) return;
    float dist;
    bool intersects = shape->intersectionTest(ray, dist);
    shape->deselect();
    if (!intersects || dist >= minDist) return;
    minDist = dist;
    selectedShape = shape;
}

void GameView::fillDisplayedBoard() {
    idxtype rows = model.getRows(),
        cols = model.getColumns();
    for (idxtype i = 0; i < rows; ++i)
        for (idxtype j = 0; j < cols; ++j) {
            const Cell& cell = model.getCellAt({i, j});
            bool playable = cell.isPlayable();
            if (!playable) continue;
            bool jumpable = cell.isJumpableOver();
            
            // board
            CellView& dcell = displayedBoard[i][j];
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
                CheckerView dchecker;
                dchecker.checkerRef = &checker;
                dchecker.position = glm::vec3(dcell.anchorPoint + dcell.upVector * (CHECKER_HALF_HEIGHT * (2*k + 1)));
                std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cylinder>(CHECKER_HALF_WIDTH, CHECKER_HALF_WIDTH, CHECKER_HALF_HEIGHT, 32);
                newShape->setColor(getDefaultColor(getPlayerOrdinal(checker.getPlayerReference()->getSlot())).toVec3());
                newShape->translate(dchecker.position);
                dchecker.shape = std::move(newShape);
                displayedCheckers.push_back(std::move(dchecker));
            }
        }
}

GameView::GameView(GameModel& gm) : model(gm), type(BoardShapeType::Flat) {
    displayedBoard.resize(gm.getRows());
    for (auto& row : displayedBoard) {
        row.resize(gm.getColumns());
    }
    fillDisplayedBoard();
}

void GameView::draw(const Shader& shader, float currentTime) {
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

void GameView::TEMPdeselectAll() {
    for (const auto& row : displayedBoard) {
        for (const auto& dcell : row) {
            if (dcell.shape != nullptr) dcell.shape->deselect();
        }
    }
    for (const auto& dchecker : displayedCheckers) {
        if (dchecker.shape != nullptr) dchecker.shape->deselect();
    }
}

void GameView::TEMPselectDistinctCell(int i, int j) {
    displayedBoard[i][j].shape->select();
}

void GameView::TEMPselectDistinctChecker(const Checker& c) {
    for (const auto& dchecker : displayedCheckers) {
        if (dchecker.checkerRef == &c) { dchecker.shape->select(); return; }
    }
}

void GameView::TEMPstageCheckerSelection(const Checker& c) {
    for (const auto& dchecker : displayedCheckers) {
        if (dchecker.checkerRef == &c) { dchecker.shape->stageSelection(); return; }
    }
}

float GameView::TEMPselectShapeByIntersection(const Ray& ray) {
    float minDist = FLT_MAX;
    OpenGLShape* selectedShape = nullptr;
    for (const auto& row : displayedBoard)
        for (const auto& dcell : row)
            checkShapeIntersection(ray, dcell.shape.get(), selectedShape, minDist);
    for (const auto& dchecker : displayedCheckers)
        checkShapeIntersection(ray, dchecker.shape.get(), selectedShape, minDist);
    if (selectedShape) selectedShape->select();
    return minDist;
}