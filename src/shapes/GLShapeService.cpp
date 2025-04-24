#include <iostream>
#include "GLShapeService.h"

const int LOG_SIZE = 512;

// false - for program. Shitcode to simplify
void checkForShaderCompilationErrors(GLuint ID, bool shader, const char* errorMessage) {
    int success;
    char infoLog[LOG_SIZE];

    if (shader) {
        glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    } else {
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
    }
    if (!success) {
        if (shader) {
            glGetShaderInfoLog(ID, LOG_SIZE, NULL, infoLog);
        } else {
            glGetProgramInfoLog(ID, LOG_SIZE, NULL, infoLog);
        }
        std::cout << errorMessage << infoLog << std::endl;
    }
}

GLuint createShaderProgram(const OpenGLShape& shape) {
    // Generate shader ID
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertexShaderCode = shape.getShaderVertices();
    const char* fragmentShaderCode = shape.getShaderFragments();
    // Attach source code to the shader
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    // Compile the shader
    glCompileShader(vertexShader);

    // Do the same for the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkForShaderCompilationErrors(vertexShader, true, "ERROR: Vertex Shader Compilation Failed\n");
    checkForShaderCompilationErrors(fragmentShader, true, "ERROR: Fragment Shader Compilation Failed\n");
    checkForShaderCompilationErrors(shaderProgram, false, "ERROR: Shader Program Linking Failed\n");
    
    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}