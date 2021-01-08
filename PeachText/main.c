#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include "glText.h"

#include "textEditorHandler.h";
#include "textCursor.h"

#include <stdlib.h>
#include <stdio.h>

unsigned int shader;


Instance* screenUI;
TEXT_EDITOR* textEditor;

void onRender() {
	PT_SCREEN_UI_render(screenUI->subInstance);
}

void onUpdate(float dt) {
	TEXT_EDITOR_update(textEditor, dt);
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
	backgroundObj->backgroundColor = PT_COLOR_fromHSV(0, 0, .3f);
	backgroundObj->size = PT_REL_DIM_new(1.0f, 0, 1.0f, 0);
	backgroundObj->zIndex = 0;

	set_instance_parent(backgroundInstance, screenUI);

	const int SIDE_BAR_WIDTH = 40;

	// main text scrollframe 
	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	scrollFrameInstance->name = create_heap_str("green frame");
	PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	scrollframe->canvasSize = PT_REL_DIM_new(0.0f, 0, 0.0f, 1000);
	PT_GUI_OBJ* scrollObj = scrollframe->guiObj;
	scrollObj->backgroundColor = backgroundObj->backgroundColor;
	scrollObj->position = PT_REL_DIM_new(0, SIDE_BAR_WIDTH, 0, 0);
	scrollObj->size = PT_REL_DIM_new(1.0f, -SIDE_BAR_WIDTH, 1.0f, 0);
	scrollObj->zIndex = 1;
	scrollObj->clipDescendants = 1;

	set_instance_parent(scrollFrameInstance, backgroundInstance);

	// menu side bar
	Instance* sideBarInstance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* sideBarObj = (PT_GUI_OBJ*)sideBarInstance->subInstance;
	sideBarObj->size = PT_REL_DIM_new(0, 220, 1.0f, 0);
	sideBarObj->anchorPosition = (vec2f){1.0f - (float)SIDE_BAR_WIDTH/(float)sideBarObj->size.xOffset, 0.0f };
	sideBarObj->position = PT_REL_DIM_new(0.0f, 0, 0, 0);
	
	sideBarObj->blurred = 1;
	sideBarObj->blurAlpha = 0.3f;
	sideBarObj->blurRadius = 30;

	sideBarObj->backgroundColor = PT_COLOR_fromRGB(0, 0, 0);

	sideBarObj->borderWidth = 1;
	sideBarObj->reactive = 1;
	sideBarObj->borderColor = PT_COLOR_fromRGB(40, 40, 40);
	sideBarObj->activeBorderColor = PT_COLOR_fromRGB(100, 100, 100);
	sideBarObj->activeBorderRange = (vec2f){ 10, 80 };
	sideBarObj->zIndex = 10;

	set_instance_parent(sideBarInstance, backgroundInstance);


	Instance* renderInstance = PT_RENDERFRAME_new();
	PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)renderInstance->subInstance;
	PT_GUI_OBJ* renderObj = renderFrame->guiObj;
	renderObj->position = PT_REL_DIM_new(0, 0, 0, 0);
	renderObj->size = PT_REL_DIM_new(0, 500, 0, 500);
	renderObj->zIndex = 5;
	renderObj->backgroundTransparency = 1;

	set_instance_parent(renderInstance, scrollFrameInstance);





	unsigned int vs = create_vertex_shader("shaders\\core\\basicQuad.vs");
	unsigned int fs = create_fragment_shader("shaders\\test.fs");
	unsigned int shaders[] = {
		vs, fs
	};

	shader = create_program(shaders, 2);






	textEditor = TEXT_EDITOR_new(scrollFrameInstance, renderFrame);

	PT_RUN(onUpdate, onRender);
}