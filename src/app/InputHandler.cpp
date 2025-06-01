#include "InputHandler.h"

std::optional<InputHandler::InputAction> InputHandler::handleKeyPress(const GLWindow& window) const {
    if (window.isPressedAlt() && window.isPressedEnter()) return InputAction::ToggleFullScreen;
    return std::nullopt;
}