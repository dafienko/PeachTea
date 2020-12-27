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

	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	scrollFrameInstance->name = create_heap_str("scrollframe");
	PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	PT_GUI_OBJ* scrollFrameObj = scrollFrame->guiObj;
	scrollFrameObj->size = PT_REL_DIM_new(.8f, 0, 1.0f, -30);
	scrollFrameObj->position = PT_REL_DIM_new(.2f, 0, 0, 30);
	scrollFrameObj->backgroundColor = PT_COLOR_fromRGB(30, 30, 30);
	scrollFrameObj->clipDescendants = 1;
	scrollFrame->canvasSize = PT_REL_DIM_new(2, 0, 2.5f, 0);

	set_instance_parent(backgroundInstance, screenUI);

	set_instance_parent(scrollFrameInstance, backgroundInstance);

	Instance* testFrame = PT_GUI_OBJ_new();
	testFrame->name = create_heap_str("red frame");
	PT_GUI_OBJ* testObj = (PT_GUI_OBJ*)testFrame->subInstance;
	testObj->backgroundColor = PT_COLOR_fromRGB(255, 0, 0);
	testObj->position = PT_REL_DIM_new(0, 95, 0, 25);
	testObj->size = PT_REL_DIM_new(0, 350, 0, 350);
	testObj->clipDescendants = 0;
	
	set_instance_parent(testFrame, scrollFrameInstance);
	
	Instance* testFrame2 = PT_GUI_OBJ_new();
	testFrame2->name = create_heap_str("green frame");
	PT_GUI_OBJ* testObj2 = (PT_GUI_OBJ*)testFrame2->subInstance;
	testObj2->anchorPosition = (vec2f){ .5f, .5f };
	testObj2->backgroundColor = PT_COLOR_fromRGB(0, 255, 0);
	testObj2->position = PT_REL_DIM_new(01, 0, 0.5, 0);
	testObj2->size = PT_REL_DIM_new(0, 200, 0, 100);

	set_instance_parent(testFrame2, testFrame);



	PT_RUN(onRender);
}