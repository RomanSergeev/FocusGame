#include <algorithm>
#include "GameSession.h"
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "model/GameModel.h"
#include "utility/Logger.h"
#include "utility/Utils.h"
#include "view/GameView.h"

const GameSession::AssumedAction GameSession::AssumedAction::NONE = GameSession::AssumedAction();

void GameSession::SelectionData::drop() {
    towerCoord = Coord::INVALID_COORD;
    whoseReserveSelected = PlayerSlot::Spectator;
    whatsSelected = SelectedEntity::None;
    for (SV& sv : selectedCheckers) {
        sv.select(HighlightState::None);
    }
    selectedCheckers.clear();
}

void GameSession::SelectionData::toggle(SV sv) {
    auto iter = std::find(selectedCheckers.begin(), selectedCheckers.end(), sv);
    if (iter == selectedCheckers.end()) {
        selectedCheckers.push_back(sv);
        sv.select(HighlightState::Selected);
    } else {
        iter->select(HighlightState::None);
        selectedCheckers.erase(iter);
        if (selectedCheckers.empty()) whatsSelected = SelectedEntity::None;
    }
}

void GameSession::SelectionData::add(SV sv) {
    selectedCheckers.push_back(sv);
    sv.select(HighlightState::Selected);
}

void GameSession::SelectionData::selectTowerTop(int amount) {
    if (whatsSelected != SelectedEntity::Tower) return;
    int i = selectedCheckers.size() - 1, end = std::max(0, i - amount + 1);
    for (; i >= end; --i)
        selectedCheckers[i].select(HighlightState::Selected);
    for (; i >= 0; --i)
        selectedCheckers[i].select(HighlightState::None);
}

void GameSession::SelectionData::selectTowerAll() { selectTowerTop(selectedCheckers.size()); }

void GameSession::SelectionData::set(const Coord& cd) {
    towerCoord = cd;
    whatsSelected = SelectedEntity::Tower;
}

void GameSession::SelectionData::set(PlayerSlot whoseReserve) {
    whoseReserveSelected = whoseReserve;
    whatsSelected = SelectedEntity::Reserve;
}

void GameSession::clearPossibleMoves() {
    for (auto iter : allPossibleMoves)
        iter.second.clear();
}

void GameSession::calculatePossibleMoves() {
    clearPossibleMoves();
    // TODO if current player is remote and isn't us - return
    // const Player& currentPlayer = model.getCurrentPlayer();
    for (idxtype i = 0; i < model.getRows(); ++i)
        for (idxtype j = 0; j < model.getColumns(); ++j) {
            Coord cd(i, j);
            allPossibleMoves[cd] = model.getPossibleMovesFor(cd);
        }
}

int GameSession::getCachedMoveDistance(const Coord& from, const Coord& to) const {
    const auto& goToPossibilities = allPossibleMoves.at(from).canGoTo;
    for (const auto& pair : goToPossibilities) {
        if (pair.first == to)
            return pair.second;
    }
    return INVALID_INDEX;
}

HighlightState GameSession::getRetainedState() const {
    bool shouldBeSelected = vectorContains(storedSelection.getCheckers(), hoveredShape);
    if (shouldBeSelected) return HighlightState::Selected;
    bool shouldBeHighlighted = vectorContains(alwaysHighlightedShapes, hoveredShape);
    if (shouldBeHighlighted) return HighlightState::CanAct;
    return HighlightState::None;
}

GameSession::AssumedAction GameSession::assumeAction() const {
    using AA = AssumedAction;
    if (hoveredShape.isEmpty()) return AA::NONE;
    using SE = SelectionData::SelectedEntity;
    SE whatsAlreadySelected = storedSelection.getSelectedEntity();
    if (vectorContains(storedSelection.getCheckers(), hoveredShape)) {
        return whatsAlreadySelected == SE::Reserve ? Action::Select : Action::Impossible; // can deselect reserve, not tower checkers currently highlighted
    }

    if (hoveredShape.isCellView()) {
        Coord cd = hoveredShape.getCellPtr()->getCoordinate();
        switch (whatsAlreadySelected) {
            case SE::None: return AA::NONE;
            case SE::Tower: {
                int distance = getCachedMoveDistance(storedSelection.getCoordinate(), cd);
                if (distance == INVALID_INDEX) return AA::NONE;
                return AA(Action::Move, distance);
            }
            case SE::Reserve: {
                int maxPlaced = allPossibleMoves.at(cd).maxReservePlaced; // considering TO cell
                bool canPlace = storedSelection.getSize() <= maxPlaced;
                return canPlace ? Action::Place : Action::Impossible; // what we've selected so far doesn't exceed what can be placed
            }
        }
        return AA::NONE;
    }
    
    // now for the checker view
    const Checker* c = hoveredShape.getCheckerPtr();
    if (c == nullptr) return AA::NONE;
    bool boardHovered = c->isOnBoard();
    const Player& currentPlayer = model.getCurrentPlayer();
    const Player* hoveredOwner = c->getPlayerReference();
    const Player* whoseReserveSelected = nullptr;
    if (whatsAlreadySelected == SE::Reserve) whoseReserveSelected = storedSelection.getCheckers().front().getCheckerPtr()->getPlayerReference(); // cringe
    if (c->isOnBoard()) {
        Coord cd = c->getPositionOnBoard();
        const Player* towerOwner = model.getCellAt(cd).getOwnership();
        bool isOwnTower = towerOwner == &currentPlayer;
        if (isOwnTower) return Action::Select;
        switch (whatsAlreadySelected) {
            case SE::None: {
                return isOwnTower ? Action::Select : Action::Impossible;
            }
            case SE::Tower: {
                if (isOwnTower) return Action::Select; // hovering our other tower
                // now hovering someone else's tower - treated as a move attempt
                int distance = getCachedMoveDistance(storedSelection.getCoordinate(), cd);
                if (distance == INVALID_INDEX) return AA::NONE;
                return { Action::Move, distance };
            }
            case SE::Reserve: {
                if (isOwnTower) return Action::Select;
                bool canPlace = model.canPlaceReserve(cd, *whoseReserveSelected, storedSelection.getSize());
                return canPlace ? Action::Place : Action::Impossible;
            }
        }
    } else { // is in tray of currentPlayer
        if (!hoveredOwner->sameTeam(currentPlayer)) return AA::NONE; // cannot interact with enemies' beaten checkers
        bool ownHovered = hoveredOwner == &currentPlayer;

        switch (whatsAlreadySelected) {
            case SE::None:
            case SE::Tower: {
                bool canSelect = ownHovered ? (model.getMaximumPlacedReserve() > 0) : (model.getCheckersTransferLimit() > 0);
                return canSelect ? Action::Select : Action::Impossible;
            }
            case SE::Reserve: {
                if (hoveredOwner == whoseReserveSelected) {
                    bool canSelectMore = storedSelection.getSize() < model.getMaximumPlacedReserve();
                    return canSelectMore ? Action::Select : Action::Impossible;
                }
                bool canSelect = ownHovered ? (model.getMaximumPlacedReserve() > 0) : (model.getCheckersTransferLimit() > 0);
                return canSelect ? Action::Select : Action::Impossible;
            }
        }
    }
}

void GameSession::onHover(const SV& sv) {
    if (sv == hoveredShape) return; // don't calculate over the same shape
    hoveredShape.select(getRetainedState());
    hoveredShape = sv;
    assumedAction = assumeAction();
    Action action = assumedAction.action;
    using HM = HighlightMethod;
    using HS = HighlightState;
    HS howToHighlightHovered = HS::None;
    if (vectorContains(storedSelection.getCheckers(), hoveredShape))
        storedSelection.selectTowerAll();
        // howToHighlightHovered = action == Action::Impossible ? HS::Selected : HS::Hovered;
    else switch (action) {
        case Action::Impossible: {
            if (highlightMethod == HM::AllOnHover && !storedSelection.isEmpty()) howToHighlightHovered = HS::CannotAct;
            else howToHighlightHovered = HS::None;
            break;
        }
        case Action::Move:
        case Action::Place: {
            if (highlightMethod == HM::Minimal) howToHighlightHovered = HS::None;
            else howToHighlightHovered = HS::CanAct;
            break;
        }
        case Action::Select: {
            howToHighlightHovered = HS::Selected;
            break;
        }
    }
    hoveredShape.select(howToHighlightHovered);
    if (assumedAction.checkersUsed > 0)
        storedSelection.selectTowerTop(assumedAction.checkersUsed);
    
}

void GameSession::onClick() { // called only when it's our turn
    if (hoveredShape.isEmpty()) return;
    using SE = SelectionData::SelectedEntity;
    SE whatsAlreadySelected = storedSelection.getSelectedEntity();
    Coord to = hoveredShape.isCellView() ? hoveredShape.getCellPtr()->getCoordinate() : hoveredShape.getCheckerPtr()->getPositionOnBoard();
    switch (assumedAction.action) {
        case Action::Impossible: return;
        case Action::Select: {
            if (whatsAlreadySelected == SE::Reserve &&
                hoveredShape.isCheckerView() &&
                !hoveredShape.getCheckerPtr()->isOnBoard() &&
                hoveredShape.getCheckerPtr()->getPlayerReference()->getSlot() == storedSelection.getAssociatedPlayer()) {
                storedSelection.toggle(hoveredShape);
                updateAlwaysHighlightedShapes();
            } else initSelection();
            break;
        }
        case Action::Move: {
            Coord from = storedSelection.getCoordinate();
            GameModel::Turn turn = GameModel::Turn::constructTurnMove(from, to);
            performTurn(turn);
            break;
        }
        case Action::Place: {
            int size = storedSelection.getSize();
            GameModel::Turn turn = GameModel::Turn::constructTurnPlace(storedSelection.getAssociatedPlayer(), size, to);
            performTurn(turn);
            break;
        }
    }
}

void GameSession::clearAllSelection() {
    clearAlwaysHighlightedCells();
    storedSelection.drop();
    pressedShape.drop(); // should not be possible to set after the turn, but just in case
    assumedAction.clear();
}

void GameSession::lockSelection() {
    selectionLocked = true;
}

void GameSession::addAlwaysHighlightedCell(SV sv) {
    sv.select(HighlightState::CanAct);
    alwaysHighlightedShapes.push_back(sv);
}

void GameSession::clearAlwaysHighlightedCells() {
    for (SV& cellView : alwaysHighlightedShapes) {
        cellView.select(HighlightState::None);
    }
    alwaysHighlightedShapes.clear();
}

void GameSession::updateAlwaysHighlightedShapes() {
    if (storedSelection.isEmpty() || highlightMethod != HighlightMethod::AllPossible) return;
    const Checker* c = storedSelection.getCheckers().front().getCheckerPtr();
    if (c == nullptr) return;
    clearAlwaysHighlightedCells();
    if (c->isOnBoard()) {
        Coord cd = c->getPositionOnBoard();
        const auto& possibleMoves = allPossibleMoves.at(cd);
        for (auto iter = possibleMoves.canGoTo.cbegin(); iter != possibleMoves.canGoTo.cend(); ++iter) {
            SV cellSV = view.getCellSV(key, iter->first);
            addAlwaysHighlightedCell(cellSV);
        }
    } else { // in reserve
        int selectedSize = storedSelection.getSize();
        for (auto iter : allPossibleMoves) {
            Coord cd = iter.first;
            int maxToPlace = iter.second.maxReservePlaced;
            if (maxToPlace >= selectedSize) {
                SV cellSV = view.getCellSV(key, cd);
                addAlwaysHighlightedCell(cellSV);
            }
        }
    }
}

void GameSession::initSelection() {
    const Checker* c = hoveredShape.getCheckerPtr();
    if (c == nullptr) return; // shouldn't be
    storedSelection.drop();
    if (highlightMethod == HighlightMethod::AllPossible) clearAlwaysHighlightedCells();
    if (c->isOnBoard()) {
        Coord cd = c->getPositionOnBoard();
        storedSelection.set(cd);
        const Cell& cell = model.getCellAt(cd);
        const CheckerContainer& checkers = cell.getCheckers(); // all checkers, including clicked-on
        int i = 0;
        for (auto iter = cell.getStartIterator(); iter != cell.getEnd(); ++iter, ++i)
            storedSelection.add(view.getCheckerSV(key, &(*iter)));
    } else {
        PlayerSlot owner = c->getPlayerReference()->getSlot();
        storedSelection.set(owner);
        storedSelection.add(hoveredShape); // just the selected shape
    }
    updateAlwaysHighlightedShapes();
}

bool GameSession::performTurn(const GameModel::Turn& turn) {
    bool result = model.makeTurn(key, turn);
    if (!result) return false;
    clearAllSelection();
    bool gameOver = model.isGameOver();
    if (gameOver) lockSelection();
    const Player& currentPlayer = model.getCurrentPlayer();
    view.updateCheckerPositions(currentPlayer.getSlot());
    view.updateOnCurrentPlayerChange(currentPlayer.getSlot());
    calculatePossibleMoves();
    return true;
}

GameSession::GameSession(GameModel&& _model, GameView::BoardShapeType type) :
model(std::move(_model)),
view(model, type) {
    for (idxtype i = 0; i < model.getRows(); ++i)
        for (idxtype j = 0; j < model.getColumns(); ++j) {
            allPossibleMoves.try_emplace({i, j}); // create default entries for each coordinate
        }
    model.start(key);
    calculatePossibleMoves();
    view.updateOnCurrentPlayerChange(model.getCurrentPlayer().getSlot());
}

void GameSession::hoverShapeByCameraRay(const Ray& ray) {
    if (selectionLocked) return;
    SV newSelected = view.getHoveredShape(key, ray);
    onHover(newSelected);
}

void GameSession::drawBoard(const Shader& shader, float currentTime) {
    PlayerSlot perspective = model.getCurrentPlayer().getSlot();
    view.draw(perspective, shader, currentTime);
}

void GameSession::handleMouseDown(const Ray& ray) {
    if (selectionLocked) return;
    pressedShape = hoveredShape; // view.getHoveredShape(key, ray); // is more robust but expensive
}

void GameSession::handleMouseUp(const Ray& ray) {
    if (selectionLocked) return;
    SV copy = pressedShape;
    pressedShape.drop();
    if (hoveredShape != copy) return; // we should release mouse hovering the same shape we pressed on
    onClick();
}

void GameSession::restart() {
    Logger::log("Restart!");
}