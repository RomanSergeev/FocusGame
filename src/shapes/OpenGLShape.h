#pragma once
#include <vector>

class OpenGLShape {
    protected:
        std::vector<float> vertices; // list of all shape's points
        std::vector<int> pointsPerAttribute; // how many attributes there are and how many floats does each attribute take (position, normale, color)
        unsigned int floatsPerAttribute; // how many floats does each (x, y, z) point carry (3 is minimal)

        const char* shaderVertices;
        const char* shaderFragments;
    public:
        OpenGLShape(unsigned int fpa) : floatsPerAttribute(fpa), pointsPerAttribute(1, fpa) {}
        virtual ~OpenGLShape() = 0;

        inline const std::vector<float>& getVertices() const { return vertices; }
        inline const std::vector<int>& getPPA() const { return pointsPerAttribute; }
        inline const char* getShaderVertices() const { return shaderVertices; }
        inline const char* getShaderFragments() const { return shaderFragments; }
        inline unsigned int getFPA() const { return floatsPerAttribute; }
};

inline OpenGLShape::~OpenGLShape() {
    // lesson learned: don't delete const char* members,
    // since they're guaranteed to have been instantiated in constructor
    // as strings of fixed length.

    //delete[] shaderVertices;
    //delete[] shaderFragments;
}