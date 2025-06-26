#include "Line.h"

Line::Line(const glm::vec3& A, const glm::vec3& B) : OpenGLShape(3) {
    update(A, B);
}

void Line::update(const glm::vec3& A, const glm::vec3& B) {
    vertices = {
        A[0], A[1], A[2],
        B[0], B[1], B[2]
    };
}