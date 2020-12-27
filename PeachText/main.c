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

	const int SIDE_PANEL_THICKNESS = 40;

	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	scrollFrameInstance->name = create_heap_str("scrollframe");
	PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	PT_GUI_OBJ* scrollFrameObj = scrollFrame->guiObj;
	scrollFrameObj->size = PT_REL_DIM_new(1.0f, -SIDE_PANEL_THICKNESS, 1.0f, 0);
	scrollFrameObj->position = PT_REL_DIM_new(0, SIDE_PANEL_THICKNESS, 0, 0);
	scrollFrameObj->backgroundColor = PT_COLOR_fromRGB(30, 30, 30);
	scrollFrameObj->clipDescendants = 1;
	scrollFrame->canvasSize = PT_REL_DIM_new(0, 0, 2.3f, 0);

	set_instance_parent(backgroundInstance, screenUI);

	set_instance_parent(scrollFrameInstance, backgroundInstance);

	Instance* sidePanel = PT_GUI_OBJ_new();
	sidePanel->name = create_heap_str("side panel");
	PT_GUI_OBJ* sideObj = (PT_GUI_OBJ*)sidePanel->subInstance;
	sideObj->backgroundColor = PT_COLOR_fromRGB(255, 0, 0);
	sideObj->position = PT_REL_DIM_new(0, 0, 0, 0);
	sideObj->size = PT_REL_DIM_new(0, SIDE_PANEL_THICKNESS, 1.0f, 0);
	sideObj->clipDescendants = 1;
	
	set_instance_parent(sidePanel, backgroundInstance);
	
	Instance* testFrame = PT_GUI_OBJ_new();
	testFrame->name = create_heap_str("green frame");
	PT_GUI_OBJ* testObj = (PT_GUI_OBJ*)testFrame->subInstance;
	testObj->anchorPosition = (vec2f){ .5f, .5f };
	testObj->backgroundColor = PT_COLOR_fromRGB(0, 255, 0);
	testObj->position = PT_REL_DIM_new(0.5f, 0, 0.5f, 0);
	testObj->size = PT_REL_DIM_new(0, 200, 0, 100);

	set_instance_parent(testFrame, scrollFrameInstance);



	PT_RUN(onRender);
}