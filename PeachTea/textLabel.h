#ifndef PT_TEXTLABEL_H
#define PT_TEXTLABEL_H

#include "guiObj.h"
#include "ScreenUI.h"

typedef enum {
	TET_STATIC, // textlabel can not be edited by user
	TET_SINGLELINE, // textlabel is only rendered as a single line
	TET_MULTILINE // textlabel is rendered with multiple lines
} TEXTLABEL_EDIT_TYPE;



typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj; // "inherit" all properties from PT_GUI_OBJ
	int visible;

	vec2i padding;

	TEXT_HORIZONTAL_ALIGNMENT horizontalAlignment;
	TEXT_VERTICAL_ALIGNMENT verticalAlignment;
	int truncateWithEllipses;

	int focused;
	
	PT_COLOR unfocusedBorderColor;
	PT_COLOR unfocusedBackgroundColor;
	PT_COLOR focusedBorderColor;
	PT_COLOR focusedBackgroundColor;

	int textSize;
	PT_FONT font;
	PT_COLOR textColor;
	PT_COLOR fringeColor;
	float textTransparency;
	
	TEXTLABEL_EDIT_TYPE editType;

	char* text;
} PT_TEXTLABEL;

Instance* PT_TEXTLABEL_new();
void PT_TEXTLABEL_destroy(void* textlabel);
PT_TEXTLABEL* PT_TEXTLABEL_clone(PT_TEXTLABEL* source, Instance* instanceClone);

PT_canvas PT_TEXTLABEL_render(PT_TEXTLABEL* textlabel, PT_SCREEN_UI* ui);
PT_canvas PT_TEXTLABEL_update_size(PT_TEXTLABEL* textlabel, PT_canvas parentCanvas);

#endif