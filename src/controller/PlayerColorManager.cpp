#include "PlayerColorManager.h"

void PlayerColorManager::loadConfiguration(const UserConfiguration& conf) {
    defaultColors = conf.getDefaultColors();
}