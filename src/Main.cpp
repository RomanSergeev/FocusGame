#include <memory>
#include "app/AppController.h"

int main() { // dummy
    std::unique_ptr<AppController> app = AppController::create();
    if (!app) return 1;

    while (!app->shouldClose()) {
        app->updateTime();
        app->handleInputMouse();
        app->handleInputKey();
        app->render();
    }
    return 0;
}