#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include <stdlib.h>
#include <stdio.h>

Instance* screenUI;
Instance* mainScrollFrameInstance;

void onRender() {
	PT_SCREEN_UI_render(screenUI->subInstance);
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

	Instance* greenFrame = PT_GUI_OBJ_new();
	greenFrame->name = create_heap_str("green frame");
	PT_GUI_OBJ* greenObj = (PT_GUI_OBJ*)greenFrame->subInstance;
	greenObj->backgroundColor = PT_COLOR_fromRGB(0, 255, 0);
	greenObj->position = PT_REL_DIM_new(0.1f, 0, 0.1f, 0);
	greenObj->size = PT_REL_DIM_new(0.45f, 0, 0.45f, 0);
	greenObj->zIndex = 1;

	set_instance_parent(greenFrame, backgroundInstance);

	Instance* blueFrame = PT_GUI_OBJ_new();
	blueFrame->name = create_heap_str("blue frame");
	PT_GUI_OBJ* blueObj = (PT_GUI_OBJ*)blueFrame->subInstance;
	blueObj->backgroundColor = PT_COLOR_fromRGB(0, 0, 255);
	blueObj->position = PT_REL_DIM_new(0.6f, 0, 0.1f, 0);
	blueObj->size = PT_REL_DIM_new(0.3, 0, 0.83f, 0);
	blueObj->zIndex = 2;
	blueObj->blurred = 1;
	blueObj->blurAlpha = .5f;
	blueObj->blurRadius = 10;

	set_instance_parent(blueFrame, backgroundInstance);

	Instance* yellowFrame = PT_GUI_OBJ_new();
	yellowFrame->name = create_heap_str("yellow frame");
	PT_GUI_OBJ* yellowObj = (PT_GUI_OBJ*)yellowFrame->subInstance;
	yellowObj->backgroundColor = PT_COLOR_fromRGB(255, 255, 0);
	yellowObj->position = PT_REL_DIM_new(0.2f, 0, 0.62f, 0);
	yellowObj->size = PT_REL_DIM_new(0.68f, 0, 0.22f, 0);
	yellowObj->zIndex = 3;

	set_instance_parent(yellowFrame, backgroundInstance);


	Instance* redFrame = PT_GUI_OBJ_new();
	redFrame->name = create_heap_str("red frame");
	PT_GUI_OBJ* redObj = (PT_GUI_OBJ*)redFrame->subInstance;
	redObj->backgroundColor = PT_COLOR_fromRGB(255, 0, 0);
	redObj->position = PT_REL_DIM_new(0.63f, 0, 0.2f, 0);
	redObj->size = PT_REL_DIM_new(0.45f, 0, 1.0f, 0);
	//redObj->size = PT_REL_DIM_new(1.0f, 0, 1.0f, 0);
	
	redObj->zIndex = 1;
	redObj->blurred = 1;
	redObj->blurAlpha = .5f;
	redObj->blurRadius = 20;

	set_instance_parent(redFrame, yellowFrame);
	//set_instance_parent(redFrame, backgroundInstance);

	PT_RUN(onRender);
}