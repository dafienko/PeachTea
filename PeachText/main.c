#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include <stdlib.h>
#include <stdio.h>

Instance* screenUI;
Instance* mainScrollFrameInstance;

int r = 21;
int s = 3;
PT_GUI_OBJ* o;

void onRender() {
	PT_SCREEN_UI_render(screenUI->subInstance);
	o->position = PT_REL_DIM_new(0, mousePos.x, 0, mousePos.y);
}

void on_scroll_up(void* arg) {
	r+=s;
	r = min(r, 45);

	o->blurRadius = r;
}

void on_scroll_down(void* arg) {
	r-=s;
	r = max(r, 1);

	o->blurRadius = r;
}

int main() {
	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PeachText");

#ifndef _DEBUG 
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	screenUI = PT_SCREEN_UI_new();
	screenUI->name = create_heap_str("screenUI");
	PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
	ui->sortingType = ZST_SIBLING;

	Instance* backgroundInstance = PT_GUI_OBJ_new();
	backgroundInstance->name = create_heap_str("background");
	PT_GUI_OBJ* backgroundObj = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundObj->backgroundColor = PT_COLOR_fromRGB(60, 60, 60);
	backgroundObj->size = PT_REL_DIM_new(1.0f, 0, 1.0f, 0);
	backgroundObj->zIndex = 0;

	set_instance_parent(backgroundInstance, screenUI);

	Instance* greenFrame = PT_SCROLLFRAME_new();
	greenFrame->name = create_heap_str("green frame");
	PT_SCROLLFRAME* greenScrollframe = (PT_SCROLLFRAME*)greenFrame->subInstance;
	greenScrollframe->canvasSize = PT_REL_DIM_new(1.5f, 0, 2.5f, 0);
	PT_GUI_OBJ* greenObj = greenScrollframe->guiObj;
	greenObj->backgroundColor = PT_COLOR_fromRGB(0, 255, 0);
	greenObj->position = PT_REL_DIM_new(0.1f, 0, 0.1f, 0);
	greenObj->size = PT_REL_DIM_new(0.45f, 0, 0.45f, 0);
	greenObj->zIndex = 1;
	greenObj->clipDescendants = 1;

	set_instance_parent(greenFrame, backgroundInstance);

	Instance* blueFrame = PT_TEXTLABEL_new();
	blueFrame->name = create_heap_str("blue frame");
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)blueFrame->subInstance;
	textlabel->text = "mtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\nmtn dew mtn dew\n";
	textlabel->font = PT_FONT_CONSOLA;
	textlabel->textColor = PT_COLOR_new(1, 1, 1);
	textlabel->textSize = 20;
	textlabel->horizontalAlignment = PT_H_ALIGNMENT_CENTER;
	textlabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;

	PT_GUI_OBJ* blueObj = textlabel->guiObj;
	blueObj->backgroundColor = PT_COLOR_fromRGB(0, 0, 255);
	blueObj->position = PT_REL_DIM_new(0.6f, 0, 0.1f, 0);
	blueObj->size = PT_REL_DIM_new(0.3, 0, 0.83f, 0);
	blueObj->zIndex = 2;
	blueObj->blurred = 1;
	blueObj->blurAlpha = .5f;
	blueObj->blurRadius = 10;
	blueObj->clipDescendants = 1;

	set_instance_parent(blueFrame, backgroundInstance);

	Instance* yellowFrame = PT_GUI_OBJ_new();
	yellowFrame->name = create_heap_str("yellow frame");
	PT_GUI_OBJ* yellowObj = (PT_GUI_OBJ*)yellowFrame->subInstance;
	yellowObj->backgroundColor = PT_COLOR_fromRGB(255, 255, 0);
	yellowObj->position = PT_REL_DIM_new(0.5f, 0, 0.62f, 0);
	yellowObj->size = PT_REL_DIM_new(0.68f, 0, 0.52f, 0);
	yellowObj->zIndex = 3;

	set_instance_parent(yellowFrame, greenFrame);

	Instance* imageFrame = PT_IMAGELABEL_new();
	imageFrame->name = create_heap_str("image frame");
	PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)imageFrame->subInstance;
	imagelabel->image = PT_IMAGE_from_png("assets\\images\\mtndewlogo.png");
	PT_GUI_OBJ* imageObj = (PT_GUI_OBJ*)imagelabel->guiObj;
	imageObj->position = PT_REL_DIM_new(0.1f, 0, 0.6f, 0);
	imageObj->size = PT_REL_DIM_new(0.45f, 0, 0.3f, 0);
	imageObj->backgroundTransparency = 1.0f;
	imageObj->borderWidth = 8;
	imageObj->borderColor = PT_COLOR_fromRGB(0, 255, 0);

	set_instance_parent(imageFrame, backgroundInstance);

	Instance* redFrame = PT_GUI_OBJ_new();
	redFrame->name = create_heap_str("red frame");
	PT_GUI_OBJ* redObj = (PT_GUI_OBJ*)redFrame->subInstance;
	redObj->backgroundColor = PT_COLOR_fromRGB(255, 100, 100);
	redObj->size = PT_REL_DIM_new(.3f, 0, .3f, 0);
	PT_SIZE_CONSTRAINT_destroy(redObj->sizeConstraint);
	redObj->sizeConstraint = PT_SIZE_CONSTRAINT_aspect(1.0f, PTSC_LOCK_Y);
	redObj->anchorPosition = (vec2f){ .5f, .5f };
	redObj->borderColor = PT_COLOR_fromRGB(255, 255, 255);
	redObj->borderWidth = 4;

	redObj->zIndex = 10;
	redObj->blurred = 1;
	redObj->blurAlpha = .8f;
	redObj->blurRadius = r;

	o = redObj;

	PT_BINDABLE_EVENT_bind(&e_wheelUp, on_scroll_up);
	PT_BINDABLE_EVENT_bind(&e_wheelDown, on_scroll_down);

	set_instance_parent(redFrame, yellowFrame);
	set_instance_parent(redFrame, backgroundInstance);

	PT_RUN(onRender);
}