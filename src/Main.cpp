#include <iostream>
#include <memory>
#include "GLWindowFunctions.h"
#include "shapes/GLShapeService.h"
#include "shapes/Cuboid.h"
#include "shapes/Triangle.h"
#include "shapes/Tetrahedron.h"
#include "CameraController.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    GLFWwindow* window;
    if (initWindow(window, WIDTH, HEIGHT, "OpenGL Window") != 0) return -1;

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    // Create simple cuboid:
    // Generate and bind Vertex Array Object
    std::unique_ptr<OpenGLShape> ptr1 = std::make_unique<Tetrahedron>();
    std::unique_ptr<OpenGLShape> ptr2 = std::make_unique<Cuboid>();
    ptr1->setupBuffer();
    ptr2->setupBuffer();
    const OpenGLShape* shape1 = ptr1.get();
    const OpenGLShape* shape2 = ptr2.get();

    /*glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 0.0f, 10.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Look-at point
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );*/
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    GLuint shaderProgram = createShaderProgram(*shape1);
    glUseProgram(shaderProgram);

    glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"  ), -0.2f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation(shaderProgram, "tetraColor"), 1.0f, 0.6f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "cubeColor" ), 0.2f, 1.0f, 0.6f);

    // define locs inside the main loop for non-fixed camera
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model"     );
    GLuint viewLoc  = glGetUniformLocation(shaderProgram, "view"      );
    GLuint projLoc  = glGetUniformLocation(shaderProgram, "projection");

    CameraController controller(window, 10.0f);
    controller.setSmoothRotation(true);
    controller.setSmoothZoom(true);

    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();

        glm::mat4 modelTetra = glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.3f, 1.0f, 0.0f));

        glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
        modelCube = glm::rotate(modelCube, time, glm::vec3(0.3f, 1.0f, -0.4f));

        controller.updateView();
        glm::mat4 view = controller.getView();
        GL_CHECK(glUniformMatrix4fv(viewLoc , 1, GL_FALSE, glm::value_ptr(view)));
        GL_CHECK(glUniformMatrix4fv(projLoc , 1, GL_FALSE, glm::value_ptr(projection)));

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shape1->draw(modelLoc, modelTetra);
        shape2->draw(modelLoc, modelCube);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}