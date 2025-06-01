#include "InputHandler.h"
#include "view/GLWindow.h"

class AppController {
    GLWindow& window;
    InputHandler& inputHandler;

    bool toggledFullscreenLastFrame = false;
public:
    AppController(GLWindow& window, InputHandler& inputHandler);
    void handleInputKey();
};