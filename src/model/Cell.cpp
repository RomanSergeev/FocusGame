#include "Cell.h"

int Cell::count = 0;

const Player* Cell::getOwnership() const {
    if (checkers.empty()) return nullptr;
    return checkers.back().getPlayerReference();
}