#include <iostream>
#include "Shader.h"

std::string replaceAllMap(const std::string& source, const std::map<std::string, std::string>& replacements) {
    std::string result = source;
    for (const auto& [key, val] : replacements) {
        size_t pos = 0;
        while ((pos = result.find(key, pos)) != std::string::npos) {
            result.replace(pos, key.length(), val);
            pos += val.length();
        }
    }
    return result;
}

// map all the ShaderParams entries here
const std::map<std::string, std::string> shaderParamsMap = {
    { "$V_POS"        , ShaderParams::V_POS      },
    { "$V_NORMAL"     , ShaderParams::V_NORMAL   },
    { "$F_POS"        , ShaderParams::F_POS      },
    { "$F_NORMAL"     , ShaderParams::F_NORMAL   },
    { "$BASE_COLOR"   , ShaderParams::BASE_COLOR },
    { "$F_COLOR"      , ShaderParams::F_COLOR    },
    { "$LIGHT_DIR"    , ShaderParams::LIGHT_DIR  },
    { "$MODEL"        , ShaderParams::MODEL      },
    { "$VIEW"         , ShaderParams::VIEW       },
    { "$PROJECTION"   , ShaderParams::PROJECTION },
    { "$TIME"         , ShaderParams::TIME       },
    { "$SELECTED_FLAG", ShaderParams::SELECTED   }
};

const std::string shaderCodeVerticesRaw = R"(
    #version 330 core
    layout (location = 0) in vec3 $V_POS;
    layout (location = 1) in vec3 $V_NORMAL;

    out vec3 $F_POS;
    out vec3 $F_NORMAL;

    uniform mat4 $MODEL;
    uniform mat4 $VIEW;
    uniform mat4 $PROJECTION;

    void main() {
        $F_POS = vec3($MODEL * vec4($V_POS, 1.0)); // world space position
        $F_NORMAL = mat3(transpose(inverse($MODEL))) * $V_NORMAL; // normal in world space
        gl_Position = $PROJECTION * $VIEW * vec4($F_POS, 1.0);
    }
)";

const std::string shaderCodeFragmentsRaw = R"(
    #version 330 core
    out vec4 $F_COLOR;

    in vec3 $F_POS;
    in vec3 $F_NORMAL;

    uniform vec3 $LIGHT_DIR;
    uniform vec3 $BASE_COLOR;
    uniform float $TIME;
    uniform bool $SELECTED_FLAG;

    void main() {
        float ambient = 0.1;
        float highlight = $SELECTED_FLAG ? 0.5 + 0.5 * sin($TIME * 4.0) : 0.0;

        float brightness = max(dot(normalize($F_NORMAL), normalize(-$LIGHT_DIR)), 0.0);
        vec3 color = $BASE_COLOR * (ambient + (1 - ambient) * brightness);
        color += highlight * vec3(1.0, 1.0, 0.5);
        $F_COLOR = vec4(color, 1.0);
    }
)";

const std::string shaderCodeVerticesRaw2D = R"(
    #version 330 core
    layout (location = 0) in vec3 $V_POS;

    uniform mat4 $PROJECTION;
    uniform mat4 $VIEW;

    void main() {
        gl_Position = $PROJECTION * $VIEW * vec4($V_POS, 1.0); // No matrices, directly to clip space
    }
)";

const char* shaderCodeFragmentsRaw2D = R"(
    #version 330 core
    out vec4 $F_COLOR;
    uniform vec3 $BASE_COLOR;

    void main() {
        $F_COLOR = vec4($BASE_COLOR, 1.0);
    }
)";

const std::string shaderCodeVertices    = replaceAllMap(shaderCodeVerticesRaw   , shaderParamsMap);
const std::string shaderCodeFragments   = replaceAllMap(shaderCodeFragmentsRaw  , shaderParamsMap);
const std::string shaderCodeVertices2D  = replaceAllMap(shaderCodeVerticesRaw2D , shaderParamsMap);
const std::string shaderCodeFragments2D = replaceAllMap(shaderCodeFragmentsRaw2D, shaderParamsMap);

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

void Shader::checkForCompilationErrors(GLuint objectID, const std::string& type) {
    GLint success;
    GLchar infoLog[LOG_SIZE];

    if (type != "PROGRAM") {
        glGetShaderiv(objectID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(objectID, LOG_SIZE, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(objectID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(objectID, LOG_SIZE, nullptr, infoLog);
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