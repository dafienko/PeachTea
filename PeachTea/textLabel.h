#ifndef PT_TEXTLABEL_H
#define PT_TEXTLABEL_H

#include "guiObj.h"

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj; // "inherit" all properties from PT_GUI_OBJ
	int visible;

	vec2i padding;

	TEXT_HORIZONTAL_ALIGNMENT horizontalAlignment;
	TEXT_VERTICAL_ALIGNMENT verticalAlignment;

	int textSize;
	PT_FONT font;

	char* text;
} PT_TEXTLABEL;

Instance* PT_TEXTLABEL_new();
void PT_TEXTLABEL_destroy(void* textlabel);

PT_GUI_DIMS PT_TEXTLABEL_render(PT_TEXTLABEL* textlabel, PT_GUI_DIMS parentDims);

#endif