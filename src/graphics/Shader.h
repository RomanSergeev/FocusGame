#pragma once
#include <string>
#include "RenderEnums.h"

extern const std::string shaderCodeVertices;
extern const std::string shaderCodeFragments;
extern const std::string shaderCodeVertices2D;
extern const std::string shaderCodeFragments2D;

namespace ShaderParams {
    inline constexpr const char* V_POS      = "aPos";
    inline constexpr const char* V_NORMAL   = "aNormal";
    inline constexpr const char* F_POS      = "FragPos";
    inline constexpr const char* F_NORMAL   = "Normal";
    inline constexpr const char* BASE_COLOR = "baseColor";
    inline constexpr const char* F_COLOR    = "FragColor";
    inline constexpr const char* LIGHT_DIR  = "lightDir";
    inline constexpr const char* MODEL      = "model";
    inline constexpr const char* VIEW       = "view";
    inline constexpr const char* PROJECTION = "projection";
    inline constexpr const char* TIME       = "time";
    inline constexpr const char* SELECTED   = "isSelected";
}

class Shader {
    GLuint ID;
    GLuint compile(GLenum type, const char* src);
    void checkForCompilationErrors(GLuint objectID, ShaderType type);
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    Shader(const Shader& s) = delete;
    Shader& operator = (const Shader& s) = delete;
    Shader(Shader&& s) noexcept;
    Shader& operator = (Shader&& s) noexcept;
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