#include <iostream>
#include <memory>
#include <vector>
#include "GLWindow.h"
#include "graphics/Shader.h"
#include "shapes/Cuboid.h"
#include "shapes/Triangle.h"
#include "shapes/Tetrahedron.h"
#include "CameraController.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    std::unique_ptr<GLWindow> window;
    try {
        window = std::make_unique<GLWindow>(WIDTH, HEIGHT, "Focus Game");
    }
    catch (const std::exception& e) {
        std::cerr << "Window initialization failed: " << e.what() << std::endl;
        return -1;
    }

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    //---------- DEMO ----------//
    const char CELLS_X = 8;
    const char CELLS_Y = 8;
    const float CUBE_W = 2.0f;
    const float CUBE_D = 0.3f; // ratio
    const float REMOVE = 2 * (CELLS_X / 2 - 1.5) * (CELLS_X / 2 - 1.5) + 2.5;

    std::vector<std::unique_ptr<OpenGLShape>> board;
    for (int i = 0; i < CELLS_X; ++i) {
        for (int j = 0; j < CELLS_Y; ++j) {
            const float DISTANCE = (CELLS_X / 2 - 0.5 - i) * (CELLS_X / 2 - 0.5 - i) + (CELLS_Y / 2 - 0.5 - j) * (CELLS_Y / 2 - 0.5 - j);
            if (DISTANCE >= REMOVE) continue;
            std::unique_ptr<OpenGLShape> cell = std::make_unique<Cuboid>(CUBE_W, CUBE_W, CUBE_W * CUBE_D);
            cell->setupBuffer();
            bool even = (i + j) & 1;
            float rgb = even ? 0.2 : 0.9;
            cell->setColor(rgb, rgb, rgb);
            float x = (i - CELLS_X / 2 + 0.5) * CUBE_W;
            float y = (j - CELLS_Y / 2 + 0.5) * CUBE_W;
            cell->position(x, y, 0);
            board.push_back(std::move(cell));
        }
    }
    //---------- DEMO ----------//

    Shader shader(shaderCodeVertices.c_str(), shaderCodeFragments.c_str());
    shader.use();
    shader.setVec3(ShaderParams::LIGHT_DIR, -0.3f, -0.2f, -0.7f);

    CameraController::CameraSettings settings;
    settings.smoothRotation = true;
    settings.smoothZoom = true;
    settings.invertedHorizontalMouse = true;
    settings.invertedVerticalMouse = true;
    CameraController controller(*window, settings, 20.0f);
    controller.setZoomLimits(4.0f, 30.0f);

    float time = glfwGetTime();
    while (!window->shouldClose()) {
        window->processInput();

        float timeDelta = time;
        time = glfwGetTime();
        timeDelta = time - timeDelta;
        controller.updateView(timeDelta);
        glm::mat4 view = controller.getView();
        shader.setMat4(ShaderParams::VIEW, view);
        glm::mat4 projection = controller.getProjectionMatrix();
        shader.setMat4(ShaderParams::PROJECTION, projection);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // rotation:
        // shape->setModel(glm::rotate(shape->getBaseModel(), time, glm::vec3(0.3f, 1.0f, 0.0f)));
        for (const auto& shape : board) {
            shape->setUniforms(shader);
            shape->draw();
        }

        window->swapBuffers();
        window->pollEvents();
    }

    return 0;
}