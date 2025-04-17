#pragma once
#include <vector>

class OpenGLShape {
    protected:
        std::vector<float> vertices;
        const char* shaderVertices;
        const char* shaderFragments;
        unsigned int floatsPerAttribute;
    public:
        OpenGLShape(unsigned int fpa) : floatsPerAttribute(fpa) {}
        virtual ~OpenGLShape() = 0;

        inline const std::vector<float>& getVertices() const { return vertices; }
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