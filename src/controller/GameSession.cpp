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
        sv.select(SelectionType::NoSelection);
    }
    selectedCheckers.clear();
}

void GameSession::SelectionData::toggle(GameView::SelectableView sv) {
    auto iter = std::find(selectedCheckers.begin(), selectedCheckers.end(), sv);
    if (iter == selectedCheckers.end()) {
        selectedCheckers.push_back(sv);
        sv.select(SelectionType::Selected);
    } else {
        iter->select(SelectionType::NoSelection);
        selectedCheckers.erase(iter);
    }
}

void GameSession::SelectionData::add(GameView::SelectableView sv) {
    selectedCheckers.push_back(sv);
}

void GameSession::SelectionData::selectTowerTop(int amount) {
    if (whatsSelected != SelectedEntity::Tower) return;
    int i = selectedCheckers.size() - 1, end = std::max(0, i - amount + 1);
    for (; i >= end; --i)
        selectedCheckers[i].select(SelectionType::Selected);
    for (; i >= 0; --i)
        selectedCheckers[i].select(SelectionType::NoSelection);
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

void GameSession::onHover(const SV& sv) {
    bool wasSelected = vectorContains(storedSelection.getCheckers(), hoveredShape);
    hoveredShape.select(wasSelected ? SelectionType::Selected : SelectionType::NoSelection);
    hoveredShape = sv;
    hoveredShape.select(SelectionType::ForSelection);
}

void GameSession::onClick() { // called only when it's our turn
    if (hoveredShape.isEmpty()) return;
    using SE = SelectionData::SelectedEntity;
    SE whatsAlreadySelected = storedSelection.getSelectedEntity();
    const Player& currentPlayer = model.getCurrentPlayer();
    if (hoveredShape.isCellView()) { // clicking on a cell - something should happen only when we have stuff selected
        const Cell* cellTo = hoveredShape.getCellPtr();
        Coord to = model.getCellCoord(*cellTo);
        switch (whatsAlreadySelected) {
            case SE::None: // nothing is selected and we're clicking on a cell - nothing happens
            break;
            case SE::Tower: {// our tower is selected - perform a move if possible
            Coord from = storedSelection.getCoordinate();
            GameModel::Turn turn = GameModel::Turn::constructTurnMove(from, to);
            performTurn(turn);
            break;}
            case SE::Reserve: {
            int size = storedSelection.getCheckers().size();
            GameModel::Turn turn = GameModel::Turn::constructTurnPlace(storedSelection.getAssociatedPlayer(), size, to);
            performTurn(turn);
            break;}
        }
    } else if (hoveredShape.isCheckerView()) { // clicking on a checker - first define where it is
        /*SelectionData whatsClicked = locateChecker(hoveredShape.getCheckerPtr());
        if (whatsClicked.isEmpty()) return; // shouldn't be, but for a safety check
        const Checker* c = whatsClicked.getCheckers().front().getCheckerPtr(); // should have a single checker in it*/
        // regardless of where it is, we should first check that it's a selectable distinct checker / part of a tower
        const Checker* c = hoveredShape.getCheckerPtr();
        if (c == nullptr || !model.isSelectableChecker(*c)) return;
        bool boardClicked = c->isOnBoard();
        switch (whatsAlreadySelected) {
            case SE::None:
            initSelection();
            break;
            case SE::Tower:
            if (!boardClicked || model.locateCheckerOnBoard(*c) != storedSelection.getCoordinate())
                initSelection(); // for a tower already selected and clicked once again, do nothing
            break;
            case SE::Reserve:
            if (boardClicked) {
                initSelection();
                break;
            }
            // reserve is clicked - is it the same owner (then toggle), or another (then reselect)?
            if (c->getPlayerReference()->getSlot() == storedSelection.getAssociatedPlayer())
                storedSelection.toggle(hoveredShape); // TODO not just toggle - check for max reserve placed
            else
                initSelection();
            break;
        }
    }
    /*auto iter = std::find(selectedShapes.begin(), selectedShapes.end(), hoveredShape);
    if (iter == selectedShapes.end()) {
        selectedShapes.push_back(hoveredShape);
        hoveredShape.select(SelectionType::Selected);
    } else {
        selectedShapes.erase(iter);
        hoveredShape.select(SelectionType::ForSelection);
    }*/
}

void GameSession::clearAllSelection() {
    highlightedShapes.clear();
    storedSelection.drop();
    pressedShape.drop(); // should not be possible to set after the turn, but just in case
}

void GameSession::lockSelection() {
    selectionLocked = true;
}

void GameSession::initSelection() {
    const Checker* c = hoveredShape.getCheckerPtr();
    if (c == nullptr) return; // shouldn't be
    storedSelection.drop();
    SV sv = view.getCheckerSV(key, c);
    if (c->isOnBoard()) {
        Coord cd = model.locateCheckerOnBoard(*c);
        storedSelection.set(cd);
        const Cell& cell = model.getCellAt(cd);
        const std::vector<Checker>& checkers = cell.getCheckers(); // all checkers, including clicked-on
        for (int i = 0; i < checkers.size(); ++i)
            storedSelection.add(view.getCheckerSV(key, &checkers.at(i)));
    } else {
        PlayerSlot owner = c->getPlayerReference()->getSlot();
        storedSelection.set(owner);
        storedSelection.add(hoveredShape); // just the selected shape
    }
}

/*GameSession::SelectionData GameSession::locateChecker(const Checker* c) const {
    if (c == nullptr) return SelectionData();
    for (auto slot : allPlayers)
        if (c->isInTrayOf(slot)) return SelectionData(model.getCurrentPlayer().getSlot(), slot);
    return SelectionData(model.locateCheckerOnBoard(*c));
}*/

bool GameSession::performTurn(const GameModel::Turn& turn) {
    bool result = model.makeTurn(key, turn);
    if (!result) return false;
    clearAllSelection();
    bool gameOver = model.isGameOver();
    if (gameOver) lockSelection();
    const Player& currentPlayer = model.getCurrentPlayer();
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