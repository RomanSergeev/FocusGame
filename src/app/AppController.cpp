#include "AppController.h"

AppController::AppController(GLWindow& window, InputHandler& handler) : window(window), inputHandler(handler) {}

void AppController::handleInputKey() {
    auto action = inputHandler.handleKeyPress(window);
    if (action != InputHandler::InputAction::ToggleFullScreen) {
        toggledFullscreenLastFrame = false;
    } else {
        if (toggledFullscreenLastFrame) return;
        toggledFullscreenLastFrame = true;
        window.toggleFullscreenMode();
    }
}