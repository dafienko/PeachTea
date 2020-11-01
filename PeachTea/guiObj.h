#ifndef PT_GUI_OBJ_H
#define PT_GUI_OBJ_H

#include "Instance.h"
#include "vectorMath.h"
#include "guiUtil.h"
#include "ScreenDimension.h"
#include "Colors.h"
#include "BindableEvent.h"

typedef struct {
	Instance* instance;

	PT_REL_DIM position;
	PT_REL_DIM size;
	int visible;
	PT_ABS_DIM lastAbsoluteDim;

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

	PT_BINDABLE_EVENT e_obj_mouseEnter;
	PT_BINDABLE_EVENT e_obj_mouseLeave;
	PT_BINDABLE_EVENT e_obj_pressed;
	PT_BINDABLE_EVENT e_obj_released;
	int mouseInFrame;
	int pressed;
} PT_GUI_OBJ;

PT_ABS_DIM PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_ABS_DIM parentDims);

void PT_GUI_OBJ_destroy(void* obj);

Instance* PT_GUI_OBJ_new();



#endif