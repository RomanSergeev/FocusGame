#include <iostream>
#include "OpenGLhub.h"

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

void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
        err = glGetError();
    }
}

GLuint createShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode) {
    // Generate shader ID
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
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

void setupBuffer(GLuint& VAO, const std::vector<float>& vertices, unsigned int floatsPerAttribute) {
    GLuint VBO; // Vertex Buffer Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // Define vertex attributes
    glVertexAttribPointer(0, floatsPerAttribute, GL_FLOAT, GL_FALSE, floatsPerAttribute * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int initWindow(GLFWwindow* &window, int width, int height, const char* title) {
    // Step 1: Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Step 2: Create a GLFW window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window); // Set OpenGL context BEFORE initializing GLEW

    // Step 3: Initialize GLEW (AFTER context is set)
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewStatus) << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set a dark greenish background
    return 0;
}