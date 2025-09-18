#pragma once
#include "controller/CameraController.h"

class UserConfiguration {
    std::unordered_map<PlayerSlot, Color> defaultColors;
    CameraController::CameraSettings defaultCameraSettings;
public:
    UserConfiguration(const std::unordered_map<PlayerSlot, Color>& colors, const CameraController::CameraSettings& camera) :
        defaultColors(colors), defaultCameraSettings(camera) {}
    const std::unordered_map<PlayerSlot, Color>& getDefaultColors() const { return defaultColors; }
    const CameraController::CameraSettings& getDefaultCameraSettings() const { return defaultCameraSettings; }
};