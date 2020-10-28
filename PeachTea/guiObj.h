#ifndef PT_GUI_OBJ_H
#define PT_GUI_OBJ_H

#include "Instance.h"
#include "vectorMath.h"
#include "guiUtil.h"
#include "ScreenDimension.h"
#include "Colors.h"

typedef struct {
	Instance* instance;

	PT_REL_DIM position;
	PT_REL_DIM size;
	int visible;

	int reactive;
	PT_COLOR activeBorderColor;
	PT_COLOR activeBackgroundColor;
	vec2f activeBorderRange;
	vec2f activeBackgroundRange;

	int borderWidth;
	float borderTransparancy;
	PT_COLOR borderColor;

	float backgroundTransparency;
	PT_COLOR backgroundColor;

	void (*onMouseEnter)(void);
	void (*onMouseLeave)(void);

	void (*render)(vec2i screenSize); // should never actually be called by user, although its there just in case
	void* typeData; // varies based on the gui object type, contains data for that specific type. Half-assed encapsulation 
} PT_GUI_OBJ;

PT_ABS_DIM PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_ABS_DIM parentDims);

void PT_GUI_OBJ_destroy(void* obj);

Instance* PT_GUI_OBJ_new();



#endif