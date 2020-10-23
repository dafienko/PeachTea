#ifndef PT_GUI_UTIL
#define PT_GUI_UTIL

#include "vectorMath.h"
#include "Instance.h"

unsigned int* qVAO;
unsigned int* qVBO;
unsigned int frameProg;

typedef struct {
	vec2i absolutePos;
	vec2i absoluteSize;
} PT_GUI_DIMS;

void init_gui_util();

vec2i scale_and_offset_to_screen(vec2f scale, vec2i offset, vec2i parentSize);
void render_gui_instance(Instance* instance, PT_GUI_DIMS parentDims);


#endif