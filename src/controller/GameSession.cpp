#include <algorithm>
#include "GameSession.h"
#include "Constants.h"
#include "graphics/RenderEnums.h"
#include "model/GameModel.h"
#include "utility/Utils.h"
#include "view/GameView.h"

void GameSession::SelectionData::drop() {
    towerCoord = Coord::INVALID_COORD;
    whoseReserveSelected = PlayerSlot::Spectator;
    whatsSelected = SelectedEntity::None;
    for (SV& sv : selectedCheckers) {
        sv.select(HighlightState::None);
    }
    selectedCheckers.clear();
}

void GameSession::SelectionData::toggle(GameView::SelectableView sv) {
    auto iter = std::find(selectedCheckers.begin(), selectedCheckers.end(), sv);
    if (iter == selectedCheckers.end()) {
        selectedCheckers.push_back(sv);
        sv.select(HighlightState::Selected);
    } else {
        iter->select(HighlightState::None);
        selectedCheckers.erase(iter);
    }
}

void GameSession::SelectionData::add(GameView::SelectableView sv) {
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

std::pair<bool, int> GameSession::actionAvailable() {
    static const std::pair<bool, int> NO(false, 0);
    if (hoveredShape.isEmpty()) return NO;
    using SE = SelectionData::SelectedEntity;
    SE whatsAlreadySelected = storedSelection.getSelectedEntity();
    if (vectorContains(storedSelection.getCheckers(), hoveredShape)) {
        return { whatsAlreadySelected == SE::Reserve, 0 }; // can deselect reserve, not tower checkers currently highlighted
    }

    if (hoveredShape.isCellView()) {
        Coord cd = hoveredShape.getCellPtr()->getCoordinate();
        switch (whatsAlreadySelected) {
            case SE::None: return NO;
            case SE::Tower: {
                int distance = getCachedMoveDistance(storedSelection.getCoordinate(), cd);
                if (distance == INVALID_INDEX) return NO;
                return { true, distance };
            }
            case SE::Reserve: {
                int maxPlaced = allPossibleMoves.at(cd).maxReservePlaced; // considering TO cell
                return { storedSelection.getSize() <= maxPlaced, 0 }; // what we've selected so far doesn't exceed what can be placed
            }
        }
        return NO;
    }

    // now for the checker view
    const Checker* c = hoveredShape.getCheckerPtr();
    if (c == nullptr) return NO;
    bool boardHovered = c->isOnBoard();
    const Player& currentPlayer = model.getCurrentPlayer();
    const Player* owner = c->getPlayerReference();
    switch (whatsAlreadySelected) {
        case SE::None: {
            if (boardHovered) {
                Coord cd = c->getPositionOnBoard();
                return { model.getCellAt(cd).getOwnership() == &currentPlayer, 0 };
            } else {
                if (!owner->sameTeam(currentPlayer)) return NO; // cannot interact with enemies' beaten checkers
                if (owner == &currentPlayer) {
                    return { model.getMaximumPlacedReserve() > 0, 0 };
                }
                return { model.getCheckersTransferLimit() > 0, 0 };
            }
        }
        case SE::Tower: {
            if (boardHovered) {
                Coord cd = c->getPositionOnBoard();
                if (model.getCellAt(cd).getOwnership() == &currentPlayer) return { true, 0 }; // hovering our other tower
                // now hovering someone else's tower - treated as a move attempt
                int distance = getCachedMoveDistance(storedSelection.getCoordinate(), cd);
                if (distance == INVALID_INDEX) return NO;
                return { true, distance };
            } else {
                if (!owner->sameTeam(currentPlayer)) return NO; // cannot interact with enemies' beaten checkers
                if (owner == &currentPlayer) {
                    return { model.getMaximumPlacedReserve() > 0, 0 };
                }
                return { model.getCheckersTransferLimit() > 0, 0 };
            }
        }
        case SE::Reserve: {
            const Player* whoseReserveSelected = storedSelection.getCheckers().front().getCheckerPtr()->getPlayerReference(); // cringe
            if (boardHovered) {
                Coord cd = c->getPositionOnBoard();
                const Player* towerOwner = model.getCellAt(cd).getOwnership();
                if (towerOwner == &currentPlayer) return { true, 0 };
                return { model.canPlaceReserve(cd, *whoseReserveSelected, storedSelection.getSize()), 0 };
            } else {
                if (!owner->sameTeam(currentPlayer)) return NO;
                if (owner == whoseReserveSelected) {
                    return { storedSelection.getSize() < model.getMaximumPlacedReserve(), 0 };
                }
                if (owner == &currentPlayer) return { model.getMaximumPlacedReserve() > 0, 0 };
                return { model.getCheckersTransferLimit() > 0, 0 };
                // non-selected reserve checker is hovered, reserve is selected
                // can select if it's ours and we don't exceed model.getMaximumPlacedReserve()
                // or if it's allied and we don't exceed model.getCheckersTransferLimit()
            }
        }
    }
    return NO;
}

void GameSession::onHover(const SV& sv) {
    hoveredShape.select(getRetainedState());
    hoveredShape = sv;
    auto actionInfo = actionAvailable();
    bool canMove = actionInfo.first;
    using HS = HighlightState;
    using HL = HighlightLogic;
    HS howToHighlightHovered = HS::None;
    if (howToHighlight == HL::AllOnHover && canMove) howToHighlightHovered = HS::Hovered;
    if (canMove && (howToHighlight == HL::PossibleOnHover || howToHighlight == HL::AllOnHover)) howToHighlightHovered = HS::CanAct;
    if (!canMove && howToHighlight == HL::AllOnHover) howToHighlightHovered = HS::CannotAct;
    hoveredShape.select(howToHighlightHovered);
    if (actionInfo.second > 0)
        storedSelection.selectTowerTop(actionInfo.second);
}

void GameSession::onClick() { // called only when it's our turn
    if (hoveredShape.isEmpty()) return;
    using SE = SelectionData::SelectedEntity;
    SE whatsAlreadySelected = storedSelection.getSelectedEntity();
    // const Player& currentPlayer = model.getCurrentPlayer();
    if (hoveredShape.isCellView()) { // clicking on a cell - something should happen only when we have stuff selected
        const Cell* cellTo = hoveredShape.getCellPtr();
        Coord to = cellTo->getCoordinate();
        switch (whatsAlreadySelected) {
            case SE::None: // nothing is selected and we're clicking on a cell - nothing happens
            break;
            case SE::Tower: {// our tower is selected - perform a move if possible
            Coord from = storedSelection.getCoordinate();
            GameModel::Turn turn = GameModel::Turn::constructTurnMove(from, to);
            performTurn(turn);
            break;}
            case SE::Reserve: {
            int size = storedSelection.getSize();
            GameModel::Turn turn = GameModel::Turn::constructTurnPlace(storedSelection.getAssociatedPlayer(), size, to);
            performTurn(turn);
            break;}
        }
    } else { // clicking on a checker
        // regardless of where it is, we should first check that it's a selectable distinct checker / part of a tower
        const Checker* c = hoveredShape.getCheckerPtr();
        if (c == nullptr || !model.isSelectableChecker(*c)) return;
        bool boardClicked = c->isOnBoard();
        switch (whatsAlreadySelected) {
            case SE::None:
            initSelection();
            break;
            case SE::Tower:
            if (!boardClicked || c->getPositionOnBoard() != storedSelection.getCoordinate())
                initSelection(); // for a tower already selected and clicked once again, do nothing
            break;
            case SE::Reserve:
            if (boardClicked) {
                initSelection();
                break;
            }
            // reserve is clicked - is it the same owner (then toggle), or another (then reselect)?
            if (c->getPlayerReference()->getSlot() == storedSelection.getAssociatedPlayer()) {
                storedSelection.toggle(hoveredShape); // TODO not just toggle - check for max reserve placed
                updateHighlightedShapes();
            }
            else
                initSelection();
            break;
        }
    }
}

void GameSession::clearAllSelection() {
    alwaysHighlightedShapes.clear();
    storedSelection.drop();
    pressedShape.drop(); // should not be possible to set after the turn, but just in case
}

void GameSession::lockSelection() {
    selectionLocked = true;
}

void GameSession::updateHighlightedShapes() {
    if (storedSelection.isEmpty() || howToHighlight != HighlightLogic::AllPossible) return;
    const Checker* c = storedSelection.getCheckers().front().getCheckerPtr();
    if (c == nullptr) return;
    alwaysHighlightedShapes.clear();
    if (c->isOnBoard()) {
        Coord cd = c->getPositionOnBoard();
        const auto& possibleMoves = allPossibleMoves.at(cd);
        for (auto iter = possibleMoves.canGoTo.cbegin(); iter != possibleMoves.canGoTo.cend(); ++iter) {
            SV cellSV = view.getCellSV(key, cd);
            cellSV.select(HighlightState::CanAct);
            alwaysHighlightedShapes.push_back(cellSV);
        }
    } else { // in reserve
        int selectedSize = storedSelection.getSize();
        for (auto iter : allPossibleMoves) {
            Coord cd = iter.first;
            int maxToPlace = iter.second.maxReservePlaced;
            if (maxToPlace >= selectedSize) {
                SV cellSV = view.getCellSV(key, cd);
                cellSV.select(HighlightState::CanAct);
                alwaysHighlightedShapes.push_back(cellSV);
            }
        }
    }
}

void GameSession::initSelection() {
    const Checker* c = hoveredShape.getCheckerPtr();
    if (c == nullptr) return; // shouldn't be
    storedSelection.drop();
    if (howToHighlight == HighlightLogic::AllPossible) alwaysHighlightedShapes.clear();
    // SV sv = view.getCheckerSV(key, c);
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
    updateHighlightedShapes();
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