#ifndef PT_TEXTLABEL_H
#define PT_TEXTLABEL_H

#include "guiObj.h"

typedef enum {
	LEFT,
	CENTER_X,
	RIGHT
} TEXT_HORIZONTAL_ALIGNMENT;

typedef enum {
	TOP,
	CENTER_Y,
	BOTTOM
} TEXT_VERTICAL_ALIGNMENT;

typedef enum {
	COMIC,
	CONSOLA,
	TIMES
} FONT;

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj; // "inherit" all properties from PT_GUI_OBJ

	vec2i padding;

	TEXT_HORIZONTAL_ALIGNMENT horizontalAlignment;
	TEXT_VERTICAL_ALIGNMENT verticalAlignment;

	int textSize;
	FONT font;

	char* text;
} PT_TEXTLABEL;

Instance* PT_TEXLABEL_new();
void PT_TEXTLABEL_destroy(void* textlabel);

PT_GUI_DIMS PT_TEXTLABEL_render(PT_TEXTLABEL* textlabel, PT_GUI_DIMS parentDims);

#endif