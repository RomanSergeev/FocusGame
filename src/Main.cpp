#include <memory>
#include "app/AppController.h"
#include "utility/SingleInstanceMonitor.h"

const std::string uniqueAppName = "FocusGameAppWindow";

int main() { // dummy
    SingleInstance instance(uniqueAppName);
    if (!instance.isPrimaryInstance()) {
        SingleInstance::focusExistingInstance(uniqueAppName); // Optionally bring the existing window to front
        return 0;
    }
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