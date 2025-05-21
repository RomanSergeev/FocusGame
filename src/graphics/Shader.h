#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glew/glew.h"

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
    void setMat4 (const std::string& name, const glm::mat4& mat) const;
};