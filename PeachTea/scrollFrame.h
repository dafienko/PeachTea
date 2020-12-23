#ifndef PT_SCROLLFRAME_H
#define PT_SCROLLFRAME_H

#include "Instance.h"
#include "guiObj.h"
#include "ScreenUI.h"

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj;
	int visible;

	PT_REL_DIM canvasSize;
	vec2i canvasPosition;

	float xScrollAlpha, yScrollAlpha;

	vec2i dragStart;
	PT_GUI_OBJ* vscrollTrack;
	PT_GUI_OBJ* vscrollBar;
	PT_GUI_OBJ* hscrollTrack;
	PT_GUI_OBJ* hscrollBar;

	int reactive;
	PT_COLOR activeScrollBarColor;
	vec2f activeScrollBarColorRange;
} PT_SCROLLFRAME;

Instance* PT_SCROLLFRAME_new();
PT_SCROLLFRAME* PT_SCROLLFRAME_clone(PT_SCROLLFRAME* scrollFrame, Instance* instanceClone);

PT_canvas PT_SCROLLFRAME_render(PT_SCROLLFRAME* scrollFrame, PT_canvas parentCanvas, Z_SORTING_TYPE sortingType, int renderDescendants);

void PT_SCROLLFRAME_destroy(void* scrollFrame);


#endif