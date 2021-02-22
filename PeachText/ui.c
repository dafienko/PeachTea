#include "ui.h"
#include "PeachTea.h"

PT_SCROLLFRAME* create_editor_scrollframe() {
	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	scrollframe->canvasSize = PT_REL_DIM_new(0.0f, 0, 0.0f, 1000);
	scrollframe->scrollBarThickness = 12;

	PT_GUI_OBJ* vTrack = scrollframe->vscrollTrack;
	vTrack->backgroundTransparency = 1;
	PT_GUI_OBJ* hTrack = scrollframe->hscrollTrack;
	hTrack->backgroundTransparency = vTrack->backgroundTransparency;

	PT_GUI_OBJ* vBar = scrollframe->vscrollBar;
	vBar->backgroundColor = PT_COLOR_fromHSV(0, 0, .8f);
	vBar->backgroundTransparency = 0.15f;
	vBar->blurred = 1;
	vBar->blurAlpha = .8f;
	vBar->blurRadius = 10;
	vBar->reactive = 1;
	vBar->activeBackgroundColor = PT_COLOR_new(1, 1, 1);
	vBar->activeBackgroundRange = (vec2f){ 10, 200 };
	PT_GUI_OBJ* hBar = scrollframe->hscrollBar;
	hBar->backgroundColor = vBar->backgroundColor;
	hBar->backgroundTransparency = vBar->backgroundTransparency;
	hBar->blurred = vBar->blurred;
	hBar->blurAlpha = vBar->blurAlpha;
	hBar->blurRadius = vBar->blurRadius;
	hBar->reactive = vBar->reactive;
	hBar->activeBackgroundColor = vBar->activeBackgroundColor;
	hBar->activeBackgroundRange = vBar->activeBackgroundRange;

	PT_GUI_OBJ* scrollObj = scrollframe->guiObj;
	scrollObj->backgroundTransparency = 1;
	scrollObj->position = PT_REL_DIM_new(0, SIDE_BAR_WIDTH, 0, 0);
	scrollObj->size = PT_REL_DIM_new(1.0f, -SIDE_BAR_WIDTH, 1.0f, -STATUS_BAR_HEIGHT);
	scrollObj->zIndex = 5;
	scrollObj->clipDescendants = 1;

	return scrollframe;
}