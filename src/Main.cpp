#include <iostream>
#include <memory>
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

    // Create simple cuboid:
    // Generate and bind Vertex Array Object
    std::unique_ptr<OpenGLShape> ptr1 = std::make_unique<Tetrahedron>();
    std::unique_ptr<OpenGLShape> ptr2 = std::make_unique<Cuboid>();
    ptr1->setupBuffer();
    ptr2->setupBuffer();
    OpenGLShape* shape1 = ptr1.get();
    OpenGLShape* shape2 = ptr2.get();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    //GLuint shaderProgram = createShaderProgram(*shape1);
    Shader shader(shaderCodeVertices, shaderCodeFragments);
    shader.use();

    shader.setVec3("lightDir", -0.2f, -1.0f, -0.3f);
    GL_CHECK(shader.setMat4("projection", projection));

    // define locs inside the main loop for non-fixed camera
    //GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");

    CameraController controller(*window, 10.0f);
    controller.setSmoothRotation(true);
    controller.setSmoothZoom(true);

    while (!window->shouldClose()) {
        window->processInput();

        float time = glfwGetTime();
        shape1->setModel(glm::rotate(shape1->getBaseModel(), time, glm::vec3(0.3f, 1.0f, 0.0f)));
        shape2->setModel(glm::rotate(shape2->getBaseModel(), time, glm::vec3(0.3f, 1.0f, -0.4f)));

        controller.updateView();
        glm::mat4 view = controller.getView();
        GL_CHECK(shader.setMat4("view", view));

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shape1->setUniforms(shader);
        shape1->draw();
        shape2->setUniforms(shader);
        shape2->draw();

        window->swapBuffers();
        window->pollEvents();
    }

    return 0;
}