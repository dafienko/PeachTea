#ifndef PT_GL_UNIFORM_UTIL_H
#define PT_GL_UNIFORM_UTIL_H

#include "glExtensions.h"
#include "Colors.h"
#include "vectorMath.h"

void uniform_PT_COLOR(GLuint loc, PT_COLOR c);

void uniform_vec3f(GLuint loc, vec3f v);
void uniform_vec3i(GLuint loc, vec3i v);

void uniform_vec2f(GLuint loc, vec2f v);
void uniform_vec2i(GLuint loc, vec2i v);

#endif