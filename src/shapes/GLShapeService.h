#pragma once
#include <OpenGLHub.h>
#include "OpenGLShape.h"

GLuint createShaderProgram(const OpenGLShape& shape);
void setupBuffer(GLuint& VAO, const OpenGLShape& shape);