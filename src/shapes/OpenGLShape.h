#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "glew/glew.h"

class OpenGLShape {
    protected:
        std::vector<float> vertices; // list of all shape's points
        std::vector<int> pointsPerAttribute; // how many attributes there are and how many floats does each attribute take (position, normale, color)
        unsigned int floatsPerAttribute; // how many floats does each (x, y, z) point carry (3 is minimal)
        GLuint VAO; // Vertex Array Object

        const char* shaderVertices;
        const char* shaderFragments;

        void addFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
    public:
        OpenGLShape(unsigned int fpa) : floatsPerAttribute(fpa), pointsPerAttribute(1, fpa), vertices() {}
        virtual ~OpenGLShape() = 0;

        inline const std::vector<float>& getVertices() const { return vertices; }
        inline const std::vector<int>& getPPA() const { return pointsPerAttribute; }
        inline const char* getShaderVertices() const { return shaderVertices; }
        inline const char* getShaderFragments() const { return shaderFragments; }
        inline unsigned int getFPA() const { return floatsPerAttribute; }

        inline unsigned int getVerticeCount() const {
            unsigned int fpa = floatsPerAttribute;
            if (fpa == 0) fpa = 1;
            return vertices.size() / fpa;
        }
        inline unsigned int getTrianglesCount() const { return getVerticeCount() / 3; }

        void setupBuffer();
        void draw(GLuint modelLocation, glm::mat4 model) const;
};

inline OpenGLShape::~OpenGLShape() {
    // lesson learned: don't delete const char* members,
    // since they're guaranteed to have been instantiated in constructor
    // as strings of fixed length.

    //delete[] shaderVertices;
    //delete[] shaderFragments;
}