#pragma once
#include "OpenGLhub.h"
#define GL_CHECK(stmt) do { stmt; checkGLError(#stmt, __FILE__, __LINE__); } while (0)

// Usage: GL_CHECK(glFunctionCall(args...));
void checkGLError(const char* stmt, const char* fname, int line);
int initWindow(GLFWwindow* &window, int width, int height, const char* title);