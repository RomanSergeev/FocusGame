#include "Cell.h"

int Cell::count = 0;

const Player* Cell::getOwnership() const {
    if (checkers.empty()) return nullptr;
    return checkers.back().getPlayerReference();
}

int Cell::getCheckerPosition(const Checker* c) const {
    int i = 0;
    for (CheckerContainer::const_iterator iter = checkers.begin(); iter != checkers.end(); ++iter, ++i)
        if (&(*iter) == c)
            return i;
    return INVALID_INDEX;
}