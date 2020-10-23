#ifndef PT_GUI_OBJ_H
#define PT_GUI_OBJ_H

#include "Instance.h"
#include "vectorMath.h"
#include "guiUtil.h"

typedef struct {
	Instance* instance;

	vec2i pos_px;
	vec2f pos_percent;
	vec2i scale_px;
	vec2f scale_percent;
	int visible;

	int borderWidth;
	float borderTransparancy;
	vec3i border_color;

	float transparency;
	vec3i background_color;

	void (*onMouseEnter)(void);
	void (*onMouseLeave)(void);

	void (*render)(vec2i screenSize); // should never actually be called by user, although its there just in case
	void* typeData; // varies based on the gui object type, contains data for that specific type. Half-assed encapsulation 
} PT_GUI_OBJ;

PT_GUI_DIMS PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_GUI_DIMS parentDims);

void PT_GUI_OBJ_destroy(void* obj);

Instance* PT_GUI_OBJ_new();



#endif