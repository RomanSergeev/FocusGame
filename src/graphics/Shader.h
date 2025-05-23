#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glew/glew.h"

extern const char* shaderCodeVertices;
extern const char* shaderCodeFragments;

namespace ShaderParams {
    inline constexpr const char* BASE_COLOR = "baseColor";
    inline constexpr const char* LIGHT_DIR  = "lightDir";
    inline constexpr const char* MODEL      = "model";
    inline constexpr const char* VIEW       = "view";
    inline constexpr const char* PROJECTION = "projection";
}

class Shader {
    GLuint ID;
    GLuint compile(GLenum type, const char* src);
    void checkForCompilationErrors(GLuint object, const std::string& type);
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void use() const;
    GLuint getID() const;

    void setBool (const std::string& name, bool value) const;
    void setInt  (const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3 (const std::string& name, const glm::vec3& value) const;
    void setVec3 (const std::string& name, float x, float y, float z) const;
    void setMat4 (const std::string& name, const glm::mat4& mat) const;
};