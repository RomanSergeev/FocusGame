#include "OpenGLShape.h"
#include "glm/gtc/type_ptr.hpp"

const AABB& OpenGLShape::getBoundingBox() const {
    if (boxIsValid) return boundingBox;
    recalculateAABB();
    return boundingBox;
}

void OpenGLShape::recalculateAABB() const {
    static const glm::vec3 corners[8] = {
        {-1, -1, -1}, { 1, -1, -1}, {-1,  1, -1}, { 1,  1, -1},
        {-1, -1,  1}, { 1, -1,  1}, {-1,  1,  1}, { 1,  1,  1},
    };

    glm::vec3 minPoint = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxPoint = glm::vec3(std::numeric_limits<float>::lowest());

    glm::mat4 transform = model * baseModel;

    for (const glm::vec3& corner : corners) {
        glm::vec4 transformed = transform * glm::vec4(corner, 1.0f);
        glm::vec3 pos = glm::vec3(transformed);

        minPoint = glm::min(minPoint, pos);
        maxPoint = glm::max(maxPoint, pos);
    }

    boundingBox = AABB(minPoint, maxPoint);
    boxIsValid = true;
}

void OpenGLShape::addFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
    glm::vec3 N = glm::normalize(glm::cross(B - A, C - A));
    std::vector<float> face = {
        A.x, A.y, A.z, N.x, N.y, N.z,
        B.x, B.y, B.z, N.x, N.y, N.z,
        C.x, C.y, C.z, N.x, N.y, N.z
    };
    vertices.insert(vertices.end(), face.begin(), face.end());
}

void OpenGLShape::position(const glm::vec3& center) {
    baseModel = glm::translate(glm::mat4(1.0f), center);
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
        int ppa = pointsPerAttribute[i].second;
        glVertexAttribPointer(i, ppa, GL_FLOAT, GL_FALSE, floatsPerAttribute * sizeof(float), (void*)(totalShift * sizeof(float)));
        glEnableVertexAttribArray(i);
        totalShift += ppa;
    }
}

void OpenGLShape::setUniforms(const Shader& shader, float time) const {
    shader.setMat4 (ShaderParams::MODEL     , baseModel * model);
    shader.setVec3 (ShaderParams::BASE_COLOR, baseColor);
    shader.setFloat(ShaderParams::TIME      , time);
    shader.setBool (ShaderParams::SELECTED  , selected);
}

void OpenGLShape::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, getVerticeCount());
}