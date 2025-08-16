#pragma once
#include <vector>
#include "Checker.h"

class Cell {
    std::vector<Checker> checkers;
    bool playable = true;
    bool jumpableOver = true;
    bool pole = false; // for spherical board in the future
public:
    Cell() : checkers() {}
    Cell(bool playable, bool jumpable) : checkers(), playable(playable), jumpableOver(jumpable) {}
    Cell(const Cell& c) = delete;
    Cell& operator = (const Cell& c) = delete;
    Cell(Checker&& c) : checkers() { checkers.emplace_back(std::move(c)); }
    Cell(Cell&& c) : checkers(std::move(c.checkers)) {}

    bool isPlayable() const { return playable; }
    bool isJumpableOver() const { return jumpableOver; }
    const std::vector<Checker>& getCheckers() const { return checkers; }
    void setPlayable(const EditorKey& key, bool isPlayable) { playable = isPlayable; }
    void setJumpable(const EditorKey& key, bool isJumpable) { jumpableOver = isJumpable; }

    void append(Checker&& c) { checkers.push_back(std::move(c)); /* TODO add checks */ };
};