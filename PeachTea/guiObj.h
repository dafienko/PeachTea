#ifndef PT_GUI_OBJ_H
#define PT_GUI_OBJ_H

#include "Instance.h"
#include "vectorMath.h"
#include "guiUtil.h"
#include "ScreenDimension.h"
#include "Colors.h"
#include "BindableEvent.h"
#include "SizeConstraint.h"
#include "canvas.h"

typedef struct {
	Instance* instance;

	vec2f anchorPosition;
	PT_REL_DIM position;
	PT_REL_DIM size;
	int visible;
	int clipDescendants;
	PT_ABS_DIM lastAbsoluteDim;
	PT_SIZE_CONSTRAINT* sizeConstraint;
	char zIndex;

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

	PT_BINDABLE_EVENT e_obj_dragged;
	PT_BINDABLE_EVENT e_obj_mouseEnter;
	PT_BINDABLE_EVENT e_obj_mouseLeave;
	PT_BINDABLE_EVENT e_obj_pressed; 
	PT_BINDABLE_EVENT e_obj_released; // fired whenever mouse is released and the obj was pressed, regardless of mouse pos
	PT_BINDABLE_EVENT e_obj_activated; // fired if the mouse is pressed and released with the mouse in the obj 
	int mouseInFrame;
	int pressed;
} PT_GUI_OBJ;

PT_canvas PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_canvas parentCanvas, Z_SORTING_TYPE sortingType, int renderDescendants);

void PT_GUI_OBJ_destroy(void* obj);

Instance* PT_GUI_OBJ_new();
PT_GUI_OBJ* PT_GUI_OBJ_clone(PT_GUI_OBJ* source, Instance* instance);


#endif