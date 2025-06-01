#include <memory>
#include <optional>
#include "view/GLWindow.h"

class InputHandler {
public:
    enum class InputAction {
        ToggleFullScreen,
        Exit,
        CameraForward,
        CameraBackward,
        CameraLeft,
        CameraRight
    };
    std::optional<InputAction> handleKeyPress(const GLWindow& window) const;
};