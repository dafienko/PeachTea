#ifndef SHADERS_H
#define SHADERS_H

#include "glExtensions.h"

GLuint create_vertex_shader(const char* shaderName);
GLuint create_fragment_shader(const char* shaderName);
GLuint create_program(GLuint* shaders, int numShaders);

#endif