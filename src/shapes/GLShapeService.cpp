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

void setupBuffer(GLuint& VAO, const OpenGLShape& shape) {
    GLuint VBO; // Vertex Buffer Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const std::vector<float>& vertices = shape.getVertices();
    unsigned int floatsPerAttribute = shape.getFPA();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    const std::vector<int>& pointsPerAttribute = shape.getPPA();
    int totalShift = 0;
    for (int i = 0; i < pointsPerAttribute.size(); ++i) {
        int ppa = pointsPerAttribute[i];
        glVertexAttribPointer(i, pointsPerAttribute[i], GL_FLOAT, GL_FALSE, floatsPerAttribute * sizeof(float), (void*)(totalShift * sizeof(float)));
        glEnableVertexAttribArray(i);
        totalShift += ppa;
    }
}