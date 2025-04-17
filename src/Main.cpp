#include <iostream>
#include "GLWindowFunctions.h"
#include "shapes/GLShapeService.h"
#include "shapes/Cuboid.h"
#include "shapes/Triangle.h"
#include "shapes/Tetrahedron.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    GLFWwindow* window;
    if (initWindow(window, WIDTH, HEIGHT, "OpenGL Window") != 0) return -1;

    glDisable(GL_CULL_FACE);

    // Create simple cuboid:
    // Generate and bind Vertex Array Object
    // Step 4: Generate vertex array
    GLuint VAO;
    const OpenGLShape* shape = new Tetrahedron();
    setupBuffer(VAO, *shape);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Look-at point
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    GLuint shaderProgram = createShaderProgram(*shape);

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shaderProgram);

        float time = glfwGetTime();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.3f, 1.0f, 0.0f));

        // defining locs inside the main loop for non-fixed camera
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        GL_CHECK(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
        GL_CHECK(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
        GL_CHECK(glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)));

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shape;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}