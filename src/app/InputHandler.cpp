#include "InputHandler.h"

std::optional<InputHandler::InputAction> InputHandler::handleKeyPress(const InputStorage& storage) const {
    if (storage.isPressed("Alt") && storage.justPressed("Enter")) return InputAction::ToggleFullScreen;
    if (storage.justReleased(GLFW_KEY_R) && storage.noModifiers()) return InputAction::RestartGame;
    return std::nullopt;
}