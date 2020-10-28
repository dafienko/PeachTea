#include "glUniformUtil.h"

void uniform_PT_COLOR(GLuint loc, PT_COLOR c) {
	glUniform3f(loc, c.r, c.g, c.b);
}


void uniform_vec3f(GLuint loc, vec3f v) {
	glUniform3f(loc, v.x, v.y, v.z);
}

void uniform_vec3i(GLuint loc, vec3i v) {
	glUniform3i(loc, v.x, v.y, v.z);
}


void uniform_vec2f(GLuint loc, vec2f v) {
	glUniform2f(loc, v.x, v.y);
}

void uniform_vec2i(GLuint loc, vec2i v) {
	glUniform2i(loc, v.x, v.y);
}