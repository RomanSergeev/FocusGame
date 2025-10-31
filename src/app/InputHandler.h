#include <optional>
#include "utility/InputStorage.h"

class InputHandler {
public:
    enum class InputAction {
        ToggleFullScreen,
        RestartGame,
        Exit,
        CameraForward,
        CameraBackward,
        CameraLeft,
        CameraRight
    };
    std::optional<InputAction> handleKeyPress(const InputStorage& storage) const;
};