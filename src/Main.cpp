#include <iostream>
#include "OpenGLhub.h"
#include "Cuboid.h"
#include "Triangle.h"

int main() {
    GLFWwindow* window;
    if (initWindow(window, 800, 600, "OpenGL Window") != 0) return -1;

    glDisable(GL_CULL_FACE);

    // Create simple cuboid:
    // Generate and bind Vertex Array Object
    // Step 4: Generate vertex array
    GLuint VAO;
    setupBuffer(VAO, verticesTriangle, 3);

    /*glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Look-at point
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );*/

    GLuint shaderProgram = createShaderProgram(vertexShaderTriangle, fragmentShaderTriangle);

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shaderProgram);

        // defining projLoc and viewLoc inside the main loop for non-fixed camera
        /*GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        GL_CHECK(glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)));

        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GL_CHECK(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));*/

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}