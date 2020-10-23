#ifndef PT_SCREENSIZE_H
#define PT_SCREENSIZE_H

#include "vectorMath.h"

vec2i screenSize;
vec2i mainWindowPosition;

void PT_RESIZE(vec2i ss);

void screensize_init(vec2i ss);

#endif