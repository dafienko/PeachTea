#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi")

char_set cs;

Instance* guiObj;
Instance* guiObj2;

int mainProgramLoop();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void onRender() {
	PT_GUI_DIMS dims = { 0 };
	dims.absolutePos = (vec2i){ 0 };
	dims.absoluteSize = screenSize;

	render_gui_instance(guiObj, dims);
}

Instance* create_default_frame(Instance* parent) {
	Instance* instance = PT_GUI_OBJ_new();
	set_instance_parent(instance, parent);

	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;
	obj->background_color = (vec3i){ 0, 0, 0 };
	obj->transparency = 1;

	DWORD c = 0u;
	BOOL b = 0;
	HRESULT hr = DwmGetColorizationColor(&c, &b);
	DWORD mask = 255u;
	unsigned char a = c >> 24;
	unsigned char red = (c >> 16) & (mask);
	unsigned char green = (c >> 8) & (mask);
	unsigned char blue = (c >> 0) & (mask);

	COLORREF borderColorref = GetSysColor(COLOR_WINDOWFRAME);
	obj->border_color = (vec3i){ red, green, blue };
	obj->borderTransparancy = 0;
	obj->borderWidth = 1;

	obj->visible = TRUE;
	obj->pos_percent = (vec2f){ .1f, .1f };
	obj->scale_percent = (vec2f){ .8f, .8f };

	return instance;
}

int main() {
	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PT Main Window");

	cs = create_char_set("assets\\fonts\\comic.ttf", 24);

	guiObj = create_default_frame(NULL);
	
	guiObj2 = create_default_frame(guiObj);
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)guiObj2->subInstance;
	obj->pos_percent = (vec2f){ .5f, 0.0f };
	obj->scale_percent = (vec2f){ .5f, 1.0f };
	obj->scale_px = (vec2i){ -10, -10 };

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


