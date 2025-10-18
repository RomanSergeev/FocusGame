#pragma once
#include <list>
#include "Checker.h"
#include "utility/Utils.h"

using CheckerContainer = std::list<Checker>;

class Cell {
    static int count;

    CheckerContainer checkers; // begin == bottom checker, end-1 == top checker
    bool playable = true;
    bool jumpableOver = true;
    bool pole = false; // for spherical board in the future
    Coord coordinate = Coord::INVALID_COORD; // shortcut for the view once again, should be handled carefully
public:
    static int getInstancesCount() { return count; }

    Cell() : checkers() { ++count; }
    Cell(bool playable, bool jumpable) : checkers(), playable(playable), jumpableOver(jumpable) { ++count; }
    Cell(const Cell& c) = delete;
    Cell& operator = (const Cell& c) = delete;
    Cell(Checker&& c) : checkers() { ++count; checkers.emplace_back(std::move(c)); coordinate = c.getPositionOnBoard(); }
    Cell(Cell&& c) : checkers(std::move(c.checkers)), playable(c.playable), jumpableOver(c.jumpableOver), pole(c.pole), coordinate(c.coordinate) { ++count; }
    ~Cell() { --count; }

    bool isPlayable() const { return playable; }
    bool isJumpableOver() const { return jumpableOver; }
    bool isPole() const { return pole; }
    Coord getCoordinate() const { return coordinate; }
    const CheckerContainer& getCheckers() const { return checkers; }
    CheckerContainer& getCheckers(const SessionKey& key) { return checkers; }
    int getTowerHeight() const { return checkers.size(); }
    CheckerContainer::const_iterator getStartIterator() const { return checkers.begin(); }
    CheckerContainer::const_iterator getEnd() const { return checkers.end(); }
    
    void setCoordinate(const EditorKey& key, const Coord& cd) { coordinate = cd; }
    void setPlayable(const EditorKey& key, bool isPlayable) { playable = isPlayable; }
    void setJumpable(const EditorKey& key, bool isJumpable) { jumpableOver = isJumpable; }
    int getCheckerPosition(const Checker* c) const;

    void create(Checker&& c) { checkers.push_back(std::move(c)); };
    void transfer(CheckerContainer& cc, CheckerContainer::iterator srcFrom, CheckerContainer::iterator srcTo) { moveAndAppend(cc, checkers, srcFrom, srcTo); }
    void transfer(CheckerContainer& cc, CheckerContainer::iterator iter) { moveAndAppend(cc, checkers, iter); }
    const Player* getOwnership() const;
};