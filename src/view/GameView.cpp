#include <algorithm>
#include <memory>
#include "GameView.h"
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "utility/Defaults.h" // TODO remove, use actual settings
#include "shapes/Cuboid.h"
#include "shapes/Cylinder.h"
#include "utility/Utils.h"

const float GameView::TRAY_DIMENSIONS[3] = { CHECKER_HALF_WIDTH * 5, CHECKER_HALF_WIDTH * 3, CHECKER_HALF_HEIGHT * 4 };
const float GameView::FB_CELL_HEIGHT[2] = { FB_CELL_HEIGHT_NJ, FB_CELL_HEIGHT_J };
const float GameView::FB_CELL_ORIGIN[2] = { (FB_CELL_HEIGHT[0] - FB_CELL_HEIGHT[1]) / 2, 0 };
const float GameView::FB_CELL_ANCHOR[2] = { FB_CELL_HEIGHT[0] / 2 + FB_CELL_ORIGIN[0], FB_CELL_HEIGHT[1] / 2 + FB_CELL_ORIGIN[1] };

void checkShapeIntersection(const Ray& ray, OpenGLShape* shape, OpenGLShape* &selectedShape, float& minDist) {
    if (shape == nullptr) return;
    float dist;
    bool intersects = shape->intersectionTest(ray, dist);
    shape->deselect();
    if (!intersects || dist >= minDist) return;
    minDist = dist;
    selectedShape = shape;
}

glm::vec3 GameView::calculateTrayPosition(const Coord& boardSizes, int totalTrays, int trayIndex) {
    float CW = FB_CELL_WIDTH; // cell width
    float TW = TRAY_DIMENSIONS[0]; // tray width
    float TH = TRAY_DIMENSIONS[1]; // tray height
    float TG = TRAY_GAP; // tray gap
    float AW = boardSizes.y * CW; // available (board) width

    int M = floor((AW + TG) / (TW + TG)); // maximum trays in one row
    float y0 = -boardSizes.x * CW / 2; // y bottom line of the board

    // positioning math
    int r = trayIndex / M; // tray row
    int c = trayIndex % M; // tray column
    int m = std::min(M, totalTrays - M * r); // trays displayed on the current row
    float x0 = (1 - m) * (TW + TG) / 2; // x of the center of the first tray in the current row
    float x = x0 + c * (TW + TG);
    float y = y0 - (r + 0.5) * (TH + TG);

    return { x, y, 0 };
}

/*int GameView::getTrayIndex(PlayerSlot slot) const {
    for (int i = 0; i < displayedTrays.size(); ++i)
        if (displayedTrays[i].ofPlayer == slot) return i;
    return NO_SUCH_TRAY;
}*/

void GameView::fillDisplayedBoard() {
    idxtype rows = model.getRows(),
            cols = model.getColumns();
    for (idxtype i = 0; i < rows; ++i)
        for (idxtype j = 0; j < cols; ++j) {
            const Cell& cell = model.getCellAt({i, j});
            bool playable = cell.isPlayable();
            if (!playable) continue;
            bool jumpable = cell.isJumpableOver();
            float cellHeight = FB_CELL_HEIGHT[jumpable];
            float cellOrigin = FB_CELL_ORIGIN[jumpable];
            float cellAnchor = FB_CELL_ANCHOR[jumpable];
            
            // board
            CellView& dcell = displayedBoard[i][j];
            dcell.upVector = axisToVec3(Axis::Z);
            float x = (i - rows / 2.0 + 0.5) * FB_CELL_WIDTH;
            float y = (j - cols / 2.0 + 0.5) * FB_CELL_WIDTH;
            dcell.anchorPoint = glm::vec3(x, y, cellAnchor);
            std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cuboid>(FB_CELL_WIDTH, FB_CELL_WIDTH, cellHeight);
            bool even = (i + j) & 1;
            float rgb = even ? 0.2 : 0.9;
            newShape->setColor(rgb, rgb, rgb);
            newShape->translate(x, y, cellOrigin);
            dcell.shape = std::move(newShape);

            // checkers
            const auto& checkers = cell.getCheckers();
            for (int k = 0; k < checkers.size(); ++k) {
                const Checker& checker = checkers.at(k);
                CheckerView dchecker = createCheckerView(checker, dcell.anchorPoint + dcell.upVector * (CHECKER_HALF_HEIGHT * (2*k + 1)));
                displayedCheckers.push_back(std::move(dchecker));
            }
        }
}

GameView::CheckerView GameView::createCheckerView(const Checker& c, const glm::vec3& position) {
    CheckerView dChecker;
    dChecker.checkerRef = &c;
    dChecker.position = position;
    std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cylinder>(CHECKER_HALF_WIDTH, CHECKER_HALF_WIDTH, CHECKER_HALF_HEIGHT, 32);
    newShape->setColor(getDefaultColor(c.getPlayerReference()->getSlot()).toVec3());
    newShape->translate(dChecker.position);
    dChecker.shape = std::move(newShape);
    return dChecker;
}

GameView::TrayView GameView::createTrayView(PlayerSlot owner, PlayerSlot ofPlayer, const glm::vec3& position) {
    TrayView dTray(owner, ofPlayer);
    dTray.upVector = axisToVec3(Axis::Z);
    dTray.anchorPoint = glm::vec3(position.x, position.y, position.z + TRAY_DIMENSIONS[2] / 2);
    dTray.position = position;
    std::unique_ptr<OpenGLShape> newShape = std::make_unique<Cuboid>(TRAY_DIMENSIONS[0], TRAY_DIMENSIONS[1], TRAY_DIMENSIONS[2]);
    newShape->setColor(getDefaultColor(dTray.ofPlayer).toVec3());
    newShape->translate(dTray.position);
    dTray.shape = std::move(newShape);
    return dTray;
}

void GameView::createTrays() {
    // tray positioning is calculated for a flat board here. Recalculate when others are ready
    const auto& gameTrays = model.getTrayData();
    const Coord& sizes = model.getSizes();
    int totalTrays = gameTrays.size();
    for (const auto& playerTray : gameTrays) {
        // create all trays for current tray owner
        int i = 0;
        PlayerSlot owner = playerTray.first;
        displayedTrays.try_emplace(owner); // creates an empty entry
        for (const auto& trayOfPlayer : gameTrays) {
            PlayerSlot ofPlayer = trayOfPlayer.first;
            glm::vec3 position = calculateTrayPosition(sizes, totalTrays, i);
            // C++17 Return Value Optimization prevents assignment operator from invoking, instead, it is a copy initialization
            TrayView dTray = createTrayView(owner, ofPlayer, position);
            displayedTrays.at(owner).push_back(std::move(dTray));
            ++i;
        }
    }

    // fill trays
    for (const auto& entry : gameTrays) {
        std::map<PlayerSlot, int> amounts;
        PlayerSlot owner = entry.first;
        const auto& trays = displayedTrays.at(owner);
        for (const Checker& checker : entry.second) {
            const Player* player = checker.getPlayerReference();
            if (player == nullptr) {
                std::cerr << "GameView::createTrays: found null checker's player reference." << std::endl;
                continue;
            }
            PlayerSlot ofPlayer = player->getSlot();
            int k = amounts[ofPlayer]++; // creates map entry if not exists; takes non-incremented value; increments it then
            for (auto iter = trays.begin(); iter != trays.end(); ++iter) {
                if (iter->ofPlayer != ofPlayer) continue;
                CheckerView dchecker = createCheckerView(checker, iter->anchorPoint + iter->upVector * (CHECKER_HALF_HEIGHT * (2*k + 1)));
                displayedCheckers.push_back(std::move(dchecker));
            }
            /*int trayIndex = getTrayIndex(ofPlayer);
            if (trayIndex == NO_SUCH_TRAY) {
                std::cerr << "GameView::createTrays: found stray checker." << std::endl;
                continue;
            }

            TrayView& tray = displayedTrays.at(trayIndex);
            int size = tray.size;*/
        }
    }
}

GameView::GameView(const GameModel& gm, BoardShapeType shapeType) : model(gm), type(shapeType) {
    displayedBoard.resize(gm.getRows());
    for (auto& row : displayedBoard) {
        row.resize(gm.getColumns());
    }
    fillDisplayedBoard();
    createTrays();
}

void GameView::draw(PlayerSlot perspective, const Shader& shader, float currentTime) {
    for (const auto& row : displayedBoard) {
        for (const auto& dcell : row) {
            if (dcell.shape == nullptr) continue;
            dcell.shape->setUniforms(shader, currentTime);
            dcell.shape->draw();
        }
    }
    for (const auto& trayEntry : displayedTrays) {
        if (trayEntry.first != perspective) continue;
        for (const auto& dtray : trayEntry.second) {
            if (dtray.shape == nullptr) continue;
            dtray.shape->setUniforms(shader, currentTime);
            dtray.shape->draw();
        }
    }
    for (const auto& dchecker : displayedCheckers) {
        if (dchecker.shape == nullptr) continue;
        const Checker* ref = dchecker.checkerRef;
        if (ref == nullptr) {
            std::cerr << "GameView::draw: found dangling checker" << std::endl;
            continue;
        }
        if (!ref->isOnBoard() && !ref->isInTrayOf(perspective)) continue;
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