#ifndef PT_SCREENSIZE_H
#define PT_SCREENSIZE_H

#include "vectorMath.h"
#include "BindableEvent.h"

vec2i screenSize;
vec2i mainWindowPosition;

PT_BINDABLE_EVENT eOnResize;

void PT_RESIZE(vec2i ss);

void screensize_init(vec2i ss);

#endif