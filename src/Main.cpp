
#include <memory>
#include <vector>
#include "view/GLWindow.h"
#include "graphics/Shader.h"
#include "shapes/Cuboid.h"
#include "shapes/Triangle.h"
#include "shapes/Tetrahedron.h"
#include "controller/CameraController.h"
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