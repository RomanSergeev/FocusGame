#include "OpenGLShape.h"
#include "glm/gtc/type_ptr.hpp"

void OpenGLShape::addFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
    glm::vec3 N = glm::normalize(glm::cross(B - A, C - A));
    std::vector<float> face = {
        A.x, A.y, A.z, N.x, N.y, N.z,
        B.x, B.y, B.z, N.x, N.y, N.z,
        C.x, C.y, C.z, N.x, N.y, N.z
    };
    vertices.insert(vertices.end(), face.begin(), face.end());
}

void OpenGLShape::setupBuffer() {
    GLuint VBO; // Vertex Buffer Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    int totalShift = 0;
    for (int i = 0; i < pointsPerAttribute.size(); ++i) {
        int ppa = pointsPerAttribute[i];
        glVertexAttribPointer(i, pointsPerAttribute[i], GL_FLOAT, GL_FALSE, floatsPerAttribute * sizeof(float), (void*)(totalShift * sizeof(float)));
        glEnableVertexAttribArray(i);
        totalShift += ppa;
    }
}

void OpenGLShape::draw(GLuint modelLocation, glm::mat4 model) const {
    glBindVertexArray(VAO);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, getVerticeCount());
}