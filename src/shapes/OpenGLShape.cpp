#include <cfloat>
#include <stdexcept>
#include "OpenGLShape.h"
#include "Constants.h"
#include "glm/ext/matrix_transform.hpp"

/********** private **********/

void OpenGLShape::setupVBO() {
    glGenBuffers(1, &VBO); // create buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind it
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // fill it
}

void OpenGLShape::setupVAO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // still needs to be bound beforehand

    // Define vertex attributes
    int totalShift = 0;
    for (int i = 0; i < pointsPerAttribute.size(); ++i) {
        int ppa = pointsPerAttribute[i].second;
        glVertexAttribPointer(i, ppa, GL_FLOAT, GL_FALSE, floatsPerAttribute * sizeof(float), (void*)(totalShift * sizeof(float)));
        glEnableVertexAttribArray(i);
        totalShift += ppa;
    }

    glBindVertexArray(0);  // Important: prevent accidental state changes
}

void OpenGLShape::updateBuffer(int oldVerticesLength) const {
    int newVerticesLength = vertices.size();
    bool reassignBuffer = (newVerticesLength > oldVerticesLength);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (reassignBuffer)
        glBufferData(GL_ARRAY_BUFFER, newVerticesLength * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    else
        glBufferSubData(GL_ARRAY_BUFFER, 0, newVerticesLength * sizeof(float), vertices.data());
}

void OpenGLShape::calcInvProduct() {
    invProduct = glm::inverse(baseModel * model);
}

/********** protected **********/

bool OpenGLShape::intersectsMathematically(const Ray& ray, float& distance) const {
    glm::vec3 transformedOrigin = glm::vec3(invProduct * glm::vec4(ray.getOrigin(), 1.0f));
    glm::vec3 transformedDirection = glm::normalize(glm::vec3(invProduct * glm::vec4(ray.getDirection(), 0.0f)));
    return intersectsMathModel(Ray(transformedOrigin, transformedDirection, false), distance);
}

void OpenGLShape::setVertices(const std::vector<float>& newVertices) {
    boxIsValid = false;
    int oldLength = vertices.size();
    vertices = newVertices;
    updateBuffer(oldLength);
}

void OpenGLShape::setupBuffer() {
    setupVBO();
    setupVAO();
}

std::pair<int, int> OpenGLShape::locatePositionWithOffset() const {
    int offset = 0;
    for (const std::pair<AttributeType, int>& pair : pointsPerAttribute) {
        if (pair.first == AttributeType::Position) return { offset, pair.second };
        offset += pair.second;
    }
    return { INVALID_INDEX, INVALID_INDEX };
}

bool OpenGLShape::getVertexBoundaries(glm::vec3& minPoint, glm::vec3& maxPoint) const {
    std::pair<int, int> position = locatePositionWithOffset();
    if (position.first == INVALID_INDEX) return false;

    minPoint = glm::vec3( FLT_MAX);
    maxPoint = glm::vec3(-FLT_MAX);
    float x, y, z;
    int offset = position.first;

    for (int i = 0; i < vertices.size(); i += floatsPerAttribute) {
        x = vertices[offset + i    ];
        y = vertices[offset + i + 1];
        z = vertices[offset + i + 2];

        if (x < minPoint.x) minPoint.x = x;
        if (x > maxPoint.x) maxPoint.x = x;
        if (y < minPoint.y) minPoint.y = y;
        if (y > maxPoint.y) maxPoint.y = y;
        if (z < minPoint.z) minPoint.z = z;
        if (z > maxPoint.z) maxPoint.z = z;
    }
    return true;
}

void OpenGLShape::recalculateAABB() const {
    glm::vec3 m, M; // min and max boundaries
    // using simple option here: precalculate min-max boundaries of object's vertices
    // to form the object-spaced bounding box
    if (!getVertexBoundaries(m, M)) return;

    const glm::vec3 corners[8] = {
        { m.x, m.y, m.z }, { m.x, m.y, M.z }, { m.x, M.y, m.z }, { m.x, M.y, M.z }, 
        { M.x, m.y, m.z }, { M.x, m.y, M.z }, { M.x, M.y, m.z }, { M.x, M.y, M.z } 
    };

    glm::vec3 minPoint = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxPoint = glm::vec3(std::numeric_limits<float>::lowest());

    glm::mat4 transform = baseModel * model;

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

void OpenGLShape::addFacePoly(const std::vector<glm::vec3>& verticeList) {
    if (verticeList.size() < 3) throw std::invalid_argument("OpenGLShape::addFacePoly verticeList argument should be at least length 3.");
    const glm::vec3& A = verticeList.at(0);
    for (int i = 1; i < verticeList.size() - 1; ++i)
        addFace(A, verticeList.at(i), verticeList.at(i + 1));
}

/********** public **********/

OpenGLShape::OpenGLShape(unsigned int fpa) :
    floatsPerAttribute(fpa),
    pointsPerAttribute(1, { AttributeType::Position, fpa }),
    vertices() {}

std::ostream& operator << (std::ostream& out, const OpenGLShape& shape) {
    shape.print(out);
    return out;
}

const AABB& OpenGLShape::getBoundingBox() const {
    if (!boxIsValid) recalculateAABB();
    return boundingBox;
}

bool OpenGLShape::intersectionTest(const Ray& ray, float& distance) const {
    return ray.intersects(getBoundingBox(), distance)
    // && ray.intersects(getOrientedBoundingBox())
    && intersectsMathematically(ray, distance);
    // && ray.intersectsMesh(vertices);
}

void OpenGLShape::translate(const glm::vec3& center) {
    setBaseModel(glm::translate(glm::mat4(1.0f), center));
}

void OpenGLShape::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(getGLDrawMode(getDrawMode()), 0, getVerticeCount());
}

void OpenGLShape::setUniforms(const Shader& shader, float time) const {
    shader.setMat4 (ShaderParams::MODEL      , baseModel * model);
    shader.setVec3 (ShaderParams::BASE_COLOR , baseColor);
    shader.setVec3 (ShaderParams::BLINK_COLOR, blinkColor);
    shader.setFloat(ShaderParams::TIME       , time);
    shader.setBool (ShaderParams::SELECTED   , selected);
}

void OpenGLShape::print(std::ostream& out) const {
    int fpa = floatsPerAttribute;
    const std::vector<float>& v = vertices;
    for (int i = 0; i < v.size(); ++i) {
        out << v[i] << ' ';
        if (i % fpa == fpa - 1) out << '\n';
    }
    out << '\n';
}