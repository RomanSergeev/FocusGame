#include <algorithm>
#include <cfloat>
#include <memory>
#include <unordered_map>
#include "GameView.h"
#include "Constants.h"
#include "utility/Defaults.h"
#include "shapes/Cuboid.h"
#include "shapes/Cylinder.h"
#include "utility/Utils.h"
#include "glm/ext/matrix_transform.hpp"

const float GameView::TRAY_DIMENSIONS[3] = { CHECKER_HALF_WIDTH * 5, CHECKER_HALF_WIDTH * 3, CHECKER_HALF_HEIGHT * 4 };
const float GameView::FB_CELL_HEIGHT[2] = { FB_CELL_HEIGHT_NJ, FB_CELL_HEIGHT_J };
const float GameView::FB_CELL_ORIGIN[2] = { (FB_CELL_HEIGHT[0] - FB_CELL_HEIGHT[1]) / 2, 0 };

void selectShape(OpenGLShape* shape, SelectionType type) {
    if (shape == nullptr) return;
    shape->setBlinkColor(selectionColors.at(type));
    if (type == SelectionType::NoSelection) shape->deselect();
    else shape->select();
}

void drawShape(OpenGLShape* shape, const Shader& shader, float currentTime) {
    if (shape == nullptr) return;
    shape->setUniforms(shader, currentTime);
    shape->draw();
}

void checkShapeIntersection(const Ray& ray, const GameView::SelectableView& comparedView, GameView::SelectableView& recordedView, float& minDist) {
    if (comparedView.isEmpty()) return;
    const OpenGLShape* shape = comparedView.getShapePtr();
    if (shape == nullptr) return;
    float distance;
    bool intersects = shape->intersectionTest(ray, distance);
    if (!intersects || distance >= minDist) return;
    minDist = distance;
    recordedView = comparedView;
}

void GameView::CellView::select(SelectionType type) { selectShape(shape.get(), type); }
void GameView::CheckerView::select(SelectionType type) { selectShape(shape.get(), type); }
void GameView::SelectableView::select(SelectionType type) {
    if (cellPtr) cellPtr->select(type);
    else if (checkerPtr) checkerPtr->select(type);
}

void GameView::CheckerView::reposition(const glm::vec3& newPosition) {
    position = newPosition;
    if (shape != nullptr)
        shape->translate(newPosition);
}

void GameView::TrayView::reposition(const glm::vec3& newPosition) {
    position = newPosition;
    if (shape != nullptr)
        shape->translate(newPosition);
}

glm::vec3 GameView::calculateTrayPosition(const Coord& boardSizes, int totalTrays, int trayIndex) {
    float CW = FB_CELL_WIDTH; // cell width
    float TW = TRAY_DIMENSIONS[0]; // tray width
    float TH = TRAY_DIMENSIONS[1]; // tray height
    float TG = TRAY_GAP; // tray gap
    float AW = boardSizes.x * CW; // available (board) width

    int M = floor((AW + TG) / (TW + TG)); // maximum trays in one row
    float y0 = -boardSizes.y * CW / 2; // y bottom line of the board

    // positioning math
    int r = trayIndex / M; // tray row
    int c = trayIndex % M; // tray column
    int m = std::min(M, totalTrays - M * r); // trays displayed on the current row
    float x0 = (1 - m) * (TW + TG) / 2; // x of the center of the first tray in the current row
    float x = x0 + c * (TW + TG);
    float y = y0 - (r + 0.5) * (TH + TG);

    return { x, y, 0 };
}

GameView::CellView GameView::createCellView(const Cell& c, const glm::vec3& position, const glm::vec3& dimensions, const Color& color) {
    CellView dCell;
    dCell.cellRef = &c;
    dCell.upVector = axisToVec3(Axis::Z);
    dCell.anchorPoint = position + dCell.upVector * (dimensions.z / 2);
    std::unique_ptr<OpenGLShape> shape = std::make_unique<Cuboid>(dimensions.x, dimensions.y, dimensions.z);
    shape->setColor(color.toVec3());
    shape->translate(position);
    dCell.shape = std::move(shape);
    return dCell;
}

GameView::CheckerView GameView::createCheckerView(const Checker& c, const glm::vec3& position) {
    CheckerView dChecker;
    dChecker.checkerRef = &c;
    std::unique_ptr<OpenGLShape> shape = std::make_unique<Cylinder>(CHECKER_HALF_WIDTH, CHECKER_HALF_WIDTH, CHECKER_HALF_HEIGHT, 32);
    shape->setColor(getDefaultColor(c.getPlayerReference()->getSlot()).toVec3());
    dChecker.shape = std::move(shape);
    dChecker.reposition(position);
    return dChecker;
}

GameView::TrayView GameView::createTrayView(PlayerSlot owner, PlayerSlot ofPlayer, const glm::vec3& position) {
    TrayView dTray(owner, ofPlayer);
    dTray.upVector = axisToVec3(Axis::Z);
    dTray.anchorPoint = position + dTray.upVector * (TRAY_DIMENSIONS[2] / 2);
    std::unique_ptr<OpenGLShape> shape = std::make_unique<Cuboid>(TRAY_DIMENSIONS[0], TRAY_DIMENSIONS[1], TRAY_DIMENSIONS[2]);
    shape->setColor(getDefaultColor(ofPlayer).toVec3());
    dTray.shape = std::move(shape);
    dTray.reposition(position);
    return dTray;
}

glm::vec3 GameView::calculateCheckerPosition(const glm::vec3& anchor, const glm::vec3& up, int index) const {
    return anchor + up * (CHECKER_HALF_HEIGHT * (2*index + 1));
}

glm::vec3 GameView::calculateCheckerViewBoardPosition(const CheckerView& cv) const {
    const Checker* c = cv.checkerRef;
    Coord cd = c->getPositionOnBoard();
    int index = model.getCellAt(cd).getCheckerPosition(c);
    if (index == INVALID_INDEX) return glm::vec3();
    const CellView& clv = displayedBoard.at(cd);
    glm::vec3 anchor = clv.anchorPoint, dir = clv.upVector;
    return calculateCheckerPosition(anchor, dir, index);
}

const GameView::TrayView* GameView::findTrayView(PlayerSlot owner, PlayerSlot ofPlayer) const {
    auto iter = displayedTrays.find(owner); // TODO O(n)
    if (iter == displayedTrays.end()) return nullptr;
    for (const TrayView& tv : iter->second)
        if (tv.ofPlayer == ofPlayer)
            return &tv;
    return nullptr;
}

glm::vec3 GameView::calculateCheckerViewTrayPosition(const CheckerView& cv, PlayerSlot perspective, int index) const {
    const Checker* c = cv.checkerRef;
    const TrayView* tv = findTrayView(perspective, c->getPlayerReference()->getSlot());
    if (tv == nullptr) return glm::vec3();
    return calculateCheckerPosition(tv->anchorPoint, tv->upVector, index);
}

void GameView::updateCheckerPositions(PlayerSlot perspective) {
    std::unordered_map<PlayerSlot, int> trayAmounts; // TODO this is junk
    for (PlayerSlot slot : allPlayers)
        trayAmounts[slot] = 0;
    for (CheckerView& cv : displayedCheckers) {
        const Checker* c = cv.checkerRef;
        if (c == nullptr) continue;
        if (c->isOnBoard()) {
            cv.reposition(calculateCheckerViewBoardPosition(cv));
            continue;
        }
        if (!c->isInTrayOf(perspective)) continue;
        PlayerSlot owner = c->getPlayerReference()->getSlot();
        glm::vec3 position = calculateCheckerViewTrayPosition(cv, perspective, trayAmounts[owner]++);
        cv.reposition(position);
    }
}

void GameView::createDisplayedBoard() {
    idxtype rows = model.getRows(),
            cols = model.getColumns();
    for (idxtype i = 0; i < rows; ++i)
        for (idxtype j = 0; j < cols; ++j) {
            Coord cd(i, j);
            const Cell& cell = model.getCellAt(cd);
            bool playable = cell.isPlayable();
            if (!playable) continue;
            bool jumpable = cell.isJumpableOver();
            float x = (i - rows / 2.0 + 0.5) * FB_CELL_WIDTH;
            float y = (j - cols / 2.0 + 0.5) * FB_CELL_WIDTH;
            bool even = (i + j) & 1;
            float rgb = even ? 51 : 230;
            glm::vec3 cellPosition = { x, y, FB_CELL_ORIGIN[jumpable] };
            glm::vec3 cellDimensions = { FB_CELL_WIDTH, FB_CELL_WIDTH, FB_CELL_HEIGHT[jumpable] };
            Color cellColor(rgb, rgb, rgb);

            // board
            CellView dCell = createCellView(cell, cellPosition, cellDimensions, cellColor);
            displayedBoard.emplace(cd, std::move(dCell));

            // checkers
            const CellView& dCellNew = displayedBoard.at(cd); // cannot reference dCell anymore
            int k = 0;
            for (auto iter = cell.getStartIterator(); iter != cell.getEnd(); ++iter, ++k) {
                const Checker& checker = *iter;
                glm::vec3 position = calculateCheckerPosition(dCellNew.anchorPoint, dCellNew.upVector, k);
                CheckerView dChecker = createCheckerView(checker, position);
                displayedCheckers.push_back(std::move(dChecker));
            }
        }
    // create turn identifier - rotating pseudo-checker
    glm::vec3 position = { 0, (rows*0.25 + 0.5) * (FB_CELL_WIDTH), FB_CELL_HEIGHT_NJ / 2 };
    std::unique_ptr<OpenGLShape> shape = std::make_unique<Cylinder>(CHECKER_HALF_WIDTH, CHECKER_HALF_WIDTH, CHECKER_HALF_HEIGHT, 32);
    turnIdentifier.shape = std::move(shape);
    turnIdentifier.reposition(position);
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
            if (isNull(player, "GameView::createTrays: found null checker's player reference.")) continue;
            PlayerSlot ofPlayer = player->getSlot();
            int k = amounts[ofPlayer]++; // creates map entry if not exists; takes non-incremented value; increments it then
            for (auto iter = trays.begin(); iter != trays.end(); ++iter) {
                if (iter->ofPlayer != ofPlayer) continue;
                glm::vec3 position = calculateCheckerPosition(iter->anchorPoint, iter->upVector, k);
                CheckerView dchecker = createCheckerView(checker, position);
                displayedCheckers.push_back(std::move(dchecker));
            }
        }
    }
}

GameView::GameView(const GameModel& gm, BoardShapeType shapeType) : model(gm), type(shapeType) {
    createDisplayedBoard();
    createTrays();
}

void GameView::updateOnCurrentPlayerChange(PlayerSlot newCurrentPlayerSlot) {
    if (turnIdentifier.shape == nullptr) return;
    turnIdentifier.shape->setColor(getDefaultColor(newCurrentPlayerSlot).toVec3());
}

void GameView::draw(PlayerSlot perspective, const Shader& shader, float currentTime) {
    for (const auto& dcell : displayedBoard)
        drawShape(dcell.second.shape.get(), shader, currentTime);
    for (const auto& trayEntry : displayedTrays) {
        if (trayEntry.first != perspective) continue;
        for (const auto& dtray : trayEntry.second)
            drawShape(dtray.shape.get(), shader, currentTime);
    }
    for (const auto& dchecker : displayedCheckers) {
        const Checker* ref = dchecker.checkerRef;
        if (isNull(ref, "GameView::draw: found dangling checker")) continue;
        if (!ref->isOnBoard() && !ref->isInTrayOf(perspective)) continue;
        drawShape(dchecker.shape.get(), shader, currentTime);
    }
    turnIdentifier.shape->setModel(glm::rotate(turnIdentifier.shape->getBaseModel(), currentTime, glm::vec3(0.3f, 1.0f, 0.0f)));
    drawShape(turnIdentifier.shape.get(), shader, currentTime);
}

GameView::SelectableView GameView::getHoveredShape(const SessionKey& key, const Ray& ray) {
    float minDist = FLT_MAX;
    SelectableView result;
    for (auto& dcell : displayedBoard)
        checkShapeIntersection(ray, dcell.second, result, minDist);
    for (CheckerView& dchecker : displayedCheckers)
        checkShapeIntersection(ray, dchecker, result, minDist);
    return result;
}

GameView::SelectableView GameView::getCheckerSV(const SessionKey& key, const Checker* c) { // TODO O(n)
    if (c == nullptr) return SelectableView();
    for (CheckerView& cView : displayedCheckers) 
        if (cView.checkerRef == c)
            return SelectableView(cView);
    return SelectableView();
}