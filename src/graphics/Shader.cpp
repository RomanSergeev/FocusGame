#include <iostream>
#include "Shader.h"

const char* shaderCodeVertices = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0)); // world space position
        Normal = mat3(transpose(inverse(model))) * aNormal; // normal in world space
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* shaderCodeFragments = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 FragPos;
    in vec3 Normal;

    uniform vec3 lightDir;
    uniform vec3 baseColor;

    void main() {
        float ambient = 0.1;
        float brightness = max(dot(normalize(Normal), normalize(-lightDir)), 0.0);
        vec3 color = baseColor * (ambient + (1 - ambient) * brightness);
        FragColor = vec4(color, 1.0);
    }
)";

const int LOG_SIZE = 512;

Shader::Shader(const char* vertexSrc, const char* fragmentSrc) {
    GLuint shaderIDVertices = compile(GL_VERTEX_SHADER, vertexSrc);
    GLuint shaderIDFragments = compile(GL_FRAGMENT_SHADER, fragmentSrc);

    ID = glCreateProgram();
    glAttachShader(ID, shaderIDVertices);
    glAttachShader(ID, shaderIDFragments);
    glLinkProgram(ID);
    checkForCompilationErrors(ID, "PROGRAM");

    glDeleteShader(shaderIDVertices);
    glDeleteShader(shaderIDFragments);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkForCompilationErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

void Shader::checkForCompilationErrors(GLuint object, const std::string& type) {
    GLint success;
    GLchar infoLog[LOG_SIZE];

    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, LOG_SIZE, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, LOG_SIZE, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
        }
    }
}

void Shader::use() const {
    glUseProgram(ID);
}

GLuint Shader::getID() const {
    return ID;
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}