#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi")

Instance* screenUI;

const int default_padding = 0;
PT_COLOR accentColor = { 0 };

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void onRender() {
	PT_ABS_DIM dims = { 0 };
	dims.position = (vec2i){ 0 };
	dims.size = screenSize;

	render_gui_instance(screenUI, dims);
}

void setup_default_gui_obj(PT_GUI_OBJ* obj) {
	obj->borderColor = PT_COLOR_new(1, 1, 1);
	obj->backgroundColor = PT_COLOR_fromRGB(80, 80, 80);
	obj->backgroundTransparency = .66;

	obj->borderTransparancy = 0;
	obj->borderWidth = 1;

	obj->visible = TRUE;
}

Instance* create_default_frame(Instance* parent) {
	Instance* instance = PT_GUI_OBJ_new();
	set_instance_parent(instance, parent);

	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;
	setup_default_gui_obj(obj);

	return instance;
}


Instance* create_default_textlabel(Instance* parent) {
	Instance* instance = PT_TEXTLABEL_new();
	set_instance_parent(instance, parent);

	PT_TEXTLABEL* tlabel = (PT_TEXTLABEL*)instance->subInstance;
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)tlabel->guiObj;
	setup_default_gui_obj(obj);

	return instance;
}

Instance* create_default_imagelabel(Instance* parent) {
	Instance* instance = PT_IMAGELABEL_new();
	set_instance_parent(instance, parent);

	PT_IMAGELABEL* tlabel = (PT_IMAGELABEL*)instance->subInstance;
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)tlabel->guiObj;
	setup_default_gui_obj(obj);

	return instance;
}

void update_accent_color() {
	DWORD c = 0u;
	BOOL bool = 0;
	HRESULT hr = DwmGetColorizationColor(&c, &bool);
	unsigned char a = c >> 24;
	unsigned char r = (c >> 16) & 255u;
	unsigned char g = (c >> 8) & 255u;
	unsigned char b = (c >> 0) & 255u;

	accentColor = PT_COLOR_fromRGB( r, g, b );
}

int main() {
	update_accent_color();

	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PT Main Window");

	screenUI = PT_SCREEN_UI_new(NULL);

	Instance* backgroundInstance = create_default_frame(screenUI);
	PT_GUI_OBJ* backgroundFrame = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundFrame->borderWidth = 0;
	backgroundFrame->size = PT_REL_DIM_new(1, 0, 1, 0);
	backgroundFrame->backgroundTransparency = 0;
	backgroundFrame->backgroundColor = PT_COLOR_fromRGB(100, 100, 100);
	backgroundFrame->visible = 0;

	Instance* frameInstance = create_default_frame(screenUI);
	PT_GUI_OBJ* frame = (PT_GUI_OBJ*)frameInstance->subInstance;
	frame->visible = FALSE;
	frame->borderWidth = 0;
	frame->size = PT_REL_DIM_new(
		1, -default_padding,
		1, -default_padding
	);
	frame->position = PT_REL_DIM_new(0.5f, 0, 0.5f, 0);
	frame->visible = 0;
	frame->anchorPosition = (vec2f){ .5f, .5f };

	PT_COLOR board1 = PT_COLOR_new(0, 0, 0);
	PT_COLOR board2 = PT_COLOR_fromRGB(155, 155, 155);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			///*
			Instance* button = create_default_imagelabel(frameInstance);

			PT_IMAGELABEL* tl = (PT_IMAGELABEL*)button->subInstance;

			PT_GUI_OBJ* obj = (PT_GUI_OBJ*)tl->guiObj;

			obj->reactive = TRUE;
			obj->backgroundColor = (x + y) % 2 == 0 ? board1 : board2;
			obj->activeBackgroundColor = obj->backgroundColor; // PT_COLOR_new(1, 1, 1);
			obj->activeBorderColor = PT_COLOR_new(1, 1, 1);
			obj->activeBackgroundRange = (vec2f){ 40, 250 };
			obj->activeBorderRange = (vec2f){ 25, 70 };
			obj->backgroundTransparency = 0.0f;

			obj->position = PT_REL_DIM_new(
				((float)x) / 8.0f, 0,
				((float)y) / 8.0f, 0
			);

			obj->size = PT_REL_DIM_new(
				1.0f / 8.0f, -default_padding,
				1.0f / 8.0f, -default_padding
			);

			obj->borderColor = accentColor;
			//*/
		}
	}

	//PT_IMAGE img = PT_IMAGE_from_png("assets\\images\\chess.png");

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


