#pragma once
#define GL_CHECK(stmt) do { stmt; checkGLError(#stmt, __FILE__, __LINE__); } while (0)

#include "../include/glew/glew.h"
#include "../include/glfw/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

// Usage: GL_CHECK(glFunctionCall(args...));
void checkGLError(const char* stmt, const char* fname, int line);
GLuint createShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode);
void setupBuffer(GLuint& VAO, const std::vector<float>& vertices, unsigned int floatsPerAttribute);
int initWindow(GLFWwindow* &window, int width, int height, const char* title);