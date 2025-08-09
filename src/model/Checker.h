#pragma once
#include "Constants.h"
#include "Player.h"

class EditorKey {
    friend class BoardEditor;
    EditorKey() = default;
};

class Checker {
    static const PlayerPlaceholderID NO_PLACEHOLDER = PlayerPlaceholderID::Spectator;
    const Player* playerRef;
    PlayerPlaceholderID placeholderId;
public:
    Checker(const Player& p) : playerRef(&p) { placeholderId = NO_PLACEHOLDER; }
    Checker(const Checker& c) : playerRef(c.playerRef), placeholderId(c.placeholderId) {}
    Checker(PlayerPlaceholderID playerPlaceholderId) : playerRef(nullptr), placeholderId(playerPlaceholderId) {}

    PlayerPlaceholderID getPlaceholderID() const { return placeholderId; }
};