#pragma once
#include <vector>
#include "ShapeEnums.h"
#include "glm/glm.hpp"
#include "glew/glew.h"
#include "graphics/Shader.h"

class OpenGLShape {
    protected:
        std::vector<float> vertices; // list of all shape's points
        std::vector<std::pair<AttributeType, int>> pointsPerAttribute; // how many attributes there are and how many floats does each attribute take (position, normale, color)
        unsigned int floatsPerAttribute; // how many floats does each (x, y, z) point carry (3 is minimal)
        GLuint VAO; // Vertex Array Object
        glm::mat4 baseModel = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        void addFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
    public:
        OpenGLShape(unsigned int fpa) : floatsPerAttribute(fpa), pointsPerAttribute(1, { AttributeType::Position, fpa }), vertices() {}
        virtual ~OpenGLShape() = 0;

        inline const std::vector<float>& getVertices() const { return vertices; }
        inline const std::vector<std::pair<AttributeType, int>>& getPPA() const { return pointsPerAttribute; }
        inline unsigned int getFPA() const { return floatsPerAttribute; }
        const glm::mat4& getBaseModel() const { return baseModel; }
        const glm::mat4& getModel() const { return model; }
        void setModel(const glm::mat4& mat) { model = mat; }
        void setBaseModel(const glm::mat4& mat) { baseModel = mat; }

        inline unsigned int getVerticeCount() const {
            unsigned int fpa = floatsPerAttribute;
            if (fpa == 0) fpa = 1;
            return vertices.size() / fpa;
        }
        inline unsigned int getTrianglesCount() const { return getVerticeCount() / 3; }
        virtual ShapeType getType() const = 0;

        void setupBuffer();
        virtual void draw() const;
        virtual void setUniforms(const Shader& shader) const;
};

inline OpenGLShape::~OpenGLShape() {}