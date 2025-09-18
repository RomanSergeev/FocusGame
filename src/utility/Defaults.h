#pragma once
#include "Color.h"
#include "Constants.h"
#include "controller/CameraController.h"
#include <unordered_map>

using CamSettings = CameraController::CameraSettings;

const CamSettings defaultCameraSettings;
const std::unordered_map<std::string, float CamSettings::*> configNamesCameraFloat {
    { "sensitivity"           , &CamSettings::sensitivity      },
    { "zoomStep"              , &CamSettings::zoomStep         },
    { "zoomSmoothFactor"      , &CamSettings::zoomSmoothFactor },
    { "rotationSlowdownFactor", &CamSettings::rotateSlowdown   }
};
const std::unordered_map<std::string, bool CamSettings::*> configNamesCameraBool {
    { "smoothRotation"       , &CamSettings::smoothRotation          },
    { "smoothZoom"           , &CamSettings::smoothZoom              },
    { "mouseInvertHorizontal", &CamSettings::invertedHorizontalMouse },
    { "mouseInvertVertical"  , &CamSettings::invertedVerticalMouse   },
    { "clampYaw"             , &CamSettings::doClampYaw              }
};

const std::unordered_map<PlayerSlot, Color> defaultColors {
    { PlayerSlot::Player1, {204, 204, 204} }, // white
    { PlayerSlot::Player2, { 51,  51,  51} }, // black
    { PlayerSlot::Player3, {221,  34,  34} }, // red
    { PlayerSlot::Player4, { 34,  34, 221} }, // blue
    { PlayerSlot::Player5, { 34, 221,  34} }, // green
    { PlayerSlot::Player6, { 34, 221, 221} }, // cyan
    { PlayerSlot::Player7, {221, 221,  34} }, // yellow
    { PlayerSlot::Player8, {221,  34, 221} }  // magenta
};

inline Color getDefaultColor(PlayerSlot slot) {
    if (slot == PlayerSlot::Spectator) return Color::BLACK;
    return defaultColors.at(slot);
}

const std::unordered_map<PlayerSlot, std::string> configNamesPlayer {
    { PlayerSlot::Player1, "player1" },
    { PlayerSlot::Player2, "player2" },
    { PlayerSlot::Player3, "player3" },
    { PlayerSlot::Player4, "player4" },
    { PlayerSlot::Player5, "player5" },
    { PlayerSlot::Player6, "player6" },
    { PlayerSlot::Player7, "player7" },
    { PlayerSlot::Player8, "player8" }
};