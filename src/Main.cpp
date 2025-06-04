
#include <memory>
#include "app/AppController.h"

int main() {
    std::unique_ptr<AppController> app = AppController::create();
    if (!app) return 1;

    while (!app->shouldClose()) {
        app->updateTime();
        app->handleInputKey();
        app->render();
    }
    return 0;
}