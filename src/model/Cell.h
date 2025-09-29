#pragma once
#include <vector>
#include "Checker.h"

class Cell {
    static int count;

    std::vector<Checker> checkers; // begin == bottom checker, end-1 == top checker
    bool playable = true;
    bool jumpableOver = true;
    bool pole = false; // for spherical board in the future
public:
    static int getInstancesCount() { return count; }

    Cell() : checkers() { ++count; }
    Cell(bool playable, bool jumpable) : checkers(), playable(playable), jumpableOver(jumpable) { ++count; }
    Cell(const Cell& c) = delete;
    Cell& operator = (const Cell& c) = delete;
    Cell(Checker&& c) : checkers() { ++count; checkers.emplace_back(std::move(c)); }
    Cell(Cell&& c) : checkers(std::move(c.checkers)), playable(c.playable), jumpableOver(c.jumpableOver), pole(c.pole) { ++count; }
    ~Cell() { --count; }

    bool isPlayable() const { return playable; }
    bool isJumpableOver() const { return jumpableOver; }
    bool isPole() const { return pole; }
    const std::vector<Checker>& getCheckers() const { return checkers; }
    std::vector<Checker>& getCheckers(const SessionKey& key) { return checkers; }
    int getTowerHeight() const { return checkers.size(); }
    
    void setPlayable(const EditorKey& key, bool isPlayable) { playable = isPlayable; }
    void setJumpable(const EditorKey& key, bool isJumpable) { jumpableOver = isJumpable; }

    void append(Checker&& c) { checkers.push_back(std::move(c)); /* TODO add checks */ };
    const Player* getOwnership() const;
};