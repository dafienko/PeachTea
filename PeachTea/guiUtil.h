#ifndef PT_GUI_UTIL
#define PT_GUI_UTIL

#include "vectorMath.h"
#include "Instance.h"
#include "ScreenDimension.h"

typedef enum {
	ZST_SIBLING,
	ZST_GLOBAL
} Z_SORTING_TYPE;

typedef enum {
	PT_H_ALIGNMENT_LEFT,
	PT_H_ALIGNMENT_CENTER,
	PT_H_ALIGNMENT_RIGHT
} TEXT_HORIZONTAL_ALIGNMENT;

typedef enum {
	PT_V_ALIGNMENT_TOP,
	PT_V_ALIGNMENT_CENTER,
	PT_V_ALIGNMENT_BOTTOM
} TEXT_VERTICAL_ALIGNMENT;

typedef enum {
	PT_FONT_COMIC,
	PT_FONT_CONSOLA,
	PT_FONT_TIMES
} PT_FONT;


PT_canvas update_gui_instance_size(Instance* instance, PT_canvas parentCanvas);

void render_gui_instance(Instance* instance);

#endif