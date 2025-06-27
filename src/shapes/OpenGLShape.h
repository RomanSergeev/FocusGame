#pragma once
#include <iostream>
#include <vector>
#include "ShapeEnums.h"
#include "glm/glm.hpp"
#include "glew/glew.h"
#include "graphics/Shader.h"
#include "Utils.h"

class OpenGLShape {
    protected:
        std::vector<float> vertices; // list of all shape's points
        std::vector<std::pair<AttributeType, int>> pointsPerAttribute; // how many attributes there are and how many floats does each attribute take (position, normale, color)
        unsigned int floatsPerAttribute; // how many floats does each (x, y, z) point carry (3 is minimal)
        GLuint VAO; // Vertex Array Object
        glm::mat4 baseModel = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 baseColor = glm::vec3(1.0f);
        bool selected = false;
        mutable AABB boundingBox;
        mutable bool boxIsValid = false;

        std::pair<int, int> locatePositionWithOffset() const;
        bool getVertexBoundaries(glm::vec3& minPoint, glm::vec3& maxPoint) const;
        void recalculateAABB() const;
        void addFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
        void addFacePoly(const std::vector<glm::vec3>& verticeList);
    public:
        OpenGLShape(unsigned int fpa) : floatsPerAttribute(fpa), pointsPerAttribute(1, { AttributeType::Position, fpa }), vertices() {}
        virtual ~OpenGLShape() = 0;

        friend std::ostream& operator << (std::ostream& out, const OpenGLShape& shape);

        inline const std::vector<float>& getVertices() const { return vertices; }
        inline const std::vector<std::pair<AttributeType, int>>& getPPA() const { return pointsPerAttribute; }
        inline unsigned int getFPA() const { return floatsPerAttribute; }
        const glm::mat4& getBaseModel() const { return baseModel; }
        const glm::mat4& getModel() const { return model; }
        const glm::vec3& getColor() const { return baseColor; }
        const AABB& getBoundingBox() const;
        bool isSelected() const { return selected; }

        void setBaseModel(const glm::mat4& mat) { baseModel = mat; boxIsValid = false; }
        void setModel(const glm::mat4& mat) { model = mat; boxIsValid = false; }
        void setColor(const glm::vec3& color) { baseColor = color; }
        inline void setColor(float r, float g, float b) { setColor(glm::vec3(r, g, b)); }
        void select() { selected = true; }
        void deselect() { selected = false; }

        inline unsigned int getVerticeCount() const {
            unsigned int fpa = floatsPerAttribute;
            if (fpa == 0) fpa = 1;
            return vertices.size() / fpa;
        }
        inline unsigned int getTrianglesCount() const { return getVerticeCount() / 3; }
        virtual ShapeType getType() const = 0;
        virtual DrawMode getDrawMode() const = 0;

        void position(const glm::vec3& center);
        inline void position(float x, float y, float z) { position(glm::vec3(x, y, z)); }
        void setupBuffer();
        virtual void draw() const;
        virtual void setUniforms(const Shader& shader, float time) const;
        virtual void print(std::ostream& out) const;
};

inline OpenGLShape::~OpenGLShape() {}