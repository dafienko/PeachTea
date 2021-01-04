#include "scrollFrame.h"
#include "ScreenDimension.h"
#include "mouse.h"
#include "glExtensions.h"

#include <stdlib.h>
#include <string.h> // idk why the hell memcpy is in string
#include <stdio.h>

int SCROLL_FRAME_SIZE = 10;

void onScrollPress(void* args) {
	PT_GUI_OBJ* scrollBarObj = (PT_GUI_OBJ*)args;
	Instance* scrollFrameInstance = scrollBarObj->instance;

	PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	
	vec2i barCenter = { 0 };
	barCenter = vector_add_2i(scrollBarObj->lastAbsoluteDim.position, vector_div_2i(scrollBarObj->lastAbsoluteDim.size, 2));

	scrollFrame->dragStart = vector_sub_2i(mousePos, barCenter);
}

void onSrollDrag(void* args) {
	PT_GUI_OBJ* scrollBarObj = (PT_GUI_OBJ*)args;
	Instance* scrollFrameInstance = scrollBarObj->instance;

	PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	vec2i framePos = scrollFrame->guiObj->lastAbsoluteDim.position;
	vec2i frameSize = scrollFrame->guiObj->lastAbsoluteDim.size;
	vec2i scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, frameSize);

	int originBound = 0, endBound = 0, bwPos = 0;
	int xScroll = scrollFrame->canvasPosition.x, yScroll = scrollFrame->canvasPosition.y;
	int* affectedAxis = NULL;
	int frameSizeOffset = 0;
	int axisLength = 0;
	if (scrollFrame->vscrollBar == scrollBarObj) {
		originBound = framePos.y;
		endBound = (framePos.y + frameSize.y) - scrollFrame->scrollBarThickness;
		bwPos = mousePos.y -scrollFrame->dragStart.y;

		axisLength = scrollCanvasSize.y;
		affectedAxis = &yScroll;
		frameSizeOffset = frameSize.y / 2;
	}
	else { // horizontal scroll bar
		originBound = framePos.x;
		endBound = (framePos.x + frameSize.x) - scrollFrame->scrollBarThickness;
		bwPos = mousePos.x -scrollFrame->dragStart.x;

		axisLength = scrollCanvasSize.x;
		affectedAxis = &xScroll;
		frameSizeOffset = frameSize.x / 2;
	}

	float alpha = 0;

	if (endBound > originBound) {
		alpha = (float)(bwPos - originBound) / (float)(endBound - originBound);
	}

	*affectedAxis = (alpha * axisLength) - frameSizeOffset;
	
	vec2i targetCanvasPos = (vec2i){ xScroll, yScroll };
	targetCanvasPos.x = max(0, min(scrollCanvasSize.x - frameSize.x, targetCanvasPos.x));
	targetCanvasPos.y = max(0, min(scrollCanvasSize.y - frameSize.y, targetCanvasPos.y));

	scrollFrame->canvasPosition = targetCanvasPos;
}

Instance* PT_SCROLLFRAME_new() {
	PT_SCROLLFRAME* scrollFrame = calloc(1, sizeof(PT_SCROLLFRAME));
	scrollFrame->visible = 1;
	scrollFrame->scrollBarThickness = 10;

	Instance* instance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;

	// vertical scroll bar
	PT_GUI_OBJ* vscrollBarTrack = calloc(1, sizeof(PT_GUI_OBJ));
	vscrollBarTrack->size = PT_REL_DIM_new(0, scrollFrame->scrollBarThickness, 1.0f, -scrollFrame->scrollBarThickness);
	vscrollBarTrack->position = PT_REL_DIM_new(1.0f, 0, 0.0f, 0);
	vscrollBarTrack->anchorPosition = (vec2f){ 1.0f, 0.0f };
	vscrollBarTrack->sizeConstraint = PT_SIZE_CONSTRAINT_none();
	vscrollBarTrack->visible = 1;
	vscrollBarTrack->clipDescendants = 1;
	vscrollBarTrack->backgroundTransparency = .5f;

	PT_GUI_OBJ* vscrollBar = calloc(1, sizeof(PT_GUI_OBJ));
	vscrollBar->size = PT_REL_DIM_new(1.0f, 0, 0, 0);
	vscrollBar->position = PT_REL_DIM_new(.5f, 0, 0, 0);
	vscrollBar->anchorPosition = (vec2f){ .5f, 0.5f };
	vscrollBar->backgroundColor = PT_COLOR_fromRGB(180, 180, 180);
	vscrollBar->instance = instance;
	vscrollBar->sizeConstraint = PT_SIZE_CONSTRAINT_none();
	vscrollBar->visible = 1;

	PT_BINDABLE_EVENT_bind(&vscrollBar->e_obj_pressed, onScrollPress);
	PT_BINDABLE_EVENT_bind(&vscrollBar->e_obj_dragged, onSrollDrag);

	scrollFrame->vscrollTrack = vscrollBarTrack;
	scrollFrame->vscrollBar = vscrollBar;

	// horizontal scroll bar
	PT_GUI_OBJ* hscrollBarTrack = PT_GUI_OBJ_clone(vscrollBarTrack, NULL); //calloc(1, sizeof(PT_GUI_OBJ));
	hscrollBarTrack->size = PT_REL_DIM_new(1.0f, -scrollFrame->scrollBarThickness, 0, scrollFrame->scrollBarThickness);
	hscrollBarTrack->position = PT_REL_DIM_new(0.0f, 0, 1.0f, 0);
	hscrollBarTrack->anchorPosition = (vec2f){ 0.0f, 1.0f };

	PT_GUI_OBJ* hscrollBar = PT_GUI_OBJ_clone(vscrollBar, NULL); //calloc(1, sizeof(PT_GUI_OBJ));
	hscrollBar->size = PT_REL_DIM_new(0, 0, 1.0f, 0);
	hscrollBar->position = PT_REL_DIM_new(0, 0, 0.5f, 0);
	hscrollBar->anchorPosition = (vec2f){ 0.5f, 0.5f };
	hscrollBar->instance = instance;

	PT_BINDABLE_EVENT_bind(&hscrollBar->e_obj_pressed, onScrollPress);
	PT_BINDABLE_EVENT_bind(&hscrollBar->e_obj_dragged, onSrollDrag);

	scrollFrame->hscrollTrack = hscrollBarTrack;
	scrollFrame->hscrollBar = hscrollBar;

	scrollFrame->guiObj = obj;
	scrollFrame->instance = instance;
	instance->subInstance = (void*)scrollFrame;
	instance->instanceType = IT_SCROLLFRAME;

	return instance;
}

PT_SCROLLFRAME* PT_SCROLLFRAME_clone(PT_SCROLLFRAME* source, Instance* instanceClone) {
	PT_SCROLLFRAME* clone = calloc(1, sizeof(PT_SCROLLFRAME));

	memcpy(clone, source, sizeof(PT_SCROLLFRAME));
	
	clone->instance = instanceClone;
	clone->guiObj = PT_GUI_OBJ_clone(source->guiObj, instanceClone);

	return clone;
}

PT_canvas PT_SCROLLFRAME_update_size(PT_SCROLLFRAME* scrollFrame, PT_canvas parentCanvas) {
	PT_canvas childCanvas = PT_GUI_OBJ_update_size(scrollFrame->guiObj, parentCanvas);

	// if vscrollbar isn't visible, don't shorten hscrollbar and vice versa
	if (scrollFrame->vscrollTrack->visible) {
		scrollFrame->hscrollTrack->size = PT_REL_DIM_new(1, -scrollFrame->scrollBarThickness, 0, scrollFrame->scrollBarThickness);
	}
	else {
		scrollFrame->hscrollTrack->size = PT_REL_DIM_new(1, 0, 0, scrollFrame->scrollBarThickness);
	}

	if (scrollFrame->hscrollTrack->visible) {
		scrollFrame->vscrollTrack->size = PT_REL_DIM_new(0, scrollFrame->scrollBarThickness, 1, -scrollFrame->scrollBarThickness);
	}
	else {
		scrollFrame->vscrollTrack->size = PT_REL_DIM_new(0, scrollFrame->scrollBarThickness, 1, 0);
	}

	PT_canvas vtrackCanvas = PT_GUI_OBJ_update_size(scrollFrame->vscrollTrack, childCanvas);
	PT_canvas htrackCanvas = PT_GUI_OBJ_update_size(scrollFrame->hscrollTrack, childCanvas);


	// update scrollbar sizes and positions based on canvas position
	vec2i frameSize = canvas_size(childCanvas);
	vec2i scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, frameSize);
	vec2i canvasMidPos = vector_add_2i(scrollFrame->canvasPosition, vector_div_2i(frameSize, 2));
	vec2f canvasAlphaPos = (vec2f){ canvasMidPos.x / (float)scrollCanvasSize.x, canvasMidPos.y / (float)scrollCanvasSize.y };

	// vertical scrollbar
	if (frameSize.y < scrollCanvasSize.y) {
		int scrollRange = canvas_size(vtrackCanvas).y;
		int scrollBarHeight = max((frameSize.y / (float)scrollCanvasSize.y) * scrollRange, 20);
		int barPos = scrollRange * canvasAlphaPos.y;

		scrollFrame->vscrollBar->position = PT_REL_DIM_new(.5f, 0, 0, barPos);
		scrollFrame->vscrollBar->size = PT_REL_DIM_new(1, 0, 0, scrollBarHeight);

		scrollFrame->vscrollTrack->visible = 1;
		scrollFrame->vscrollBar->visible = 1;
	}
	else {
		scrollFrame->vscrollTrack->visible = 0;
		scrollFrame->vscrollBar->visible = 0;
	}

	// horizontal scrollbar
	if (frameSize.x < scrollCanvasSize.x) {
		int scrollRange = canvas_size(htrackCanvas).x;
		int scrollBarWidth = max((frameSize.x / (float)scrollCanvasSize.x) * scrollRange, 20);
		int barPos = scrollRange * canvasAlphaPos.x;

		scrollFrame->hscrollBar->position = PT_REL_DIM_new(0, barPos, .5f, 0);
		scrollFrame->hscrollBar->size = PT_REL_DIM_new(0, scrollBarWidth, 1, 0);

		scrollFrame->hscrollTrack->visible = 1;
		scrollFrame->hscrollBar->visible = 1;
	}
	else {
		scrollFrame->hscrollTrack->visible = 0;
		scrollFrame->hscrollBar->visible = 0;
	}


	PT_GUI_OBJ_update_size(scrollFrame->vscrollBar, vtrackCanvas);
	PT_GUI_OBJ_update_size(scrollFrame->hscrollBar, htrackCanvas);


	// offset canvas after scroll tracks and bars have been updated (don't want to scroll objects to move when canvas is moved)
	childCanvas.ox = scrollFrame->canvasPosition.x;
	childCanvas.oy = scrollFrame->canvasPosition.y;

	return childCanvas;
}

void PT_SCROLLFRAME_render(PT_SCROLLFRAME* scrollFrame, PT_SCREEN_UI* ui) {
	scrollFrame->guiObj->visible = scrollFrame->visible;

	if (scrollFrame->visible) {
		PT_GUI_OBJ_render(scrollFrame->guiObj, ui);
	}
}

void PT_SCROLLFRAME_destroy(void* obj) {
	PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)obj;

	PT_GUI_OBJ_destroy((void*)scrollFrame->guiObj);

	free(scrollFrame);
}