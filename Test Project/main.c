#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi")

char_set cs;

Instance* guiObj;
Instance* guiObj2;

vec3i accentColor = { 0 };

int mainProgramLoop();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void onRender() {
	PT_GUI_DIMS dims = { 0 };
	dims.absolutePos = (vec2i){ 0 };
	dims.absoluteSize = screenSize;

	render_gui_instance(guiObj, dims);
	render_gui_instance(guiObj2, dims);
}

Instance* create_default_frame(Instance* parent) {
	Instance* instance = PT_GUI_OBJ_new();
	set_instance_parent(instance, parent);

	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;
	obj->background_color = (vec3i){ 0, 0, 0 };
	obj->transparency = 1;

	obj->border_color = accentColor;
	obj->borderTransparancy = 0;
	obj->borderWidth = 1;

	obj->visible = TRUE;
	obj->pos_px = (vec2i){ 20, 20 };
	obj->scale_percent = (vec2f){ .5, 1 };
	obj->scale_px = (vec2i){ -30, -40 };

	return instance;
}

Instance* create_default_textlabel(Instance* parent) {
	Instance* instance = PT_TEXTLABEL_new();
	set_instance_parent(instance, parent);

	PT_TEXTLABEL* tlabel = (PT_TEXTLABEL*)instance->subInstance;
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)tlabel->guiObj;
	obj->background_color = (vec3i){ 0, 0, 0 };
	obj->transparency = 1;

	obj->border_color = accentColor;
	obj->borderTransparancy = 0;
	obj->borderWidth = 1;

	obj->visible = TRUE;
	obj->pos_percent = (vec2f){ .5, 0 };
	obj->pos_px = (vec2i){ 10, 20 };
	obj->scale_percent = (vec2f){ .5, 1 };
	obj->scale_px = (vec2i){ -30, -40 };

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

	accentColor = (vec3i){ r, g, b };
}

int main() {
	update_accent_color();
	
	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PT Main Window");

	cs = create_char_set("assets\\fonts\\comic.ttf", 24);

	guiObj = create_default_frame(NULL);
	
	guiObj2 = create_default_textlabel(NULL);
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)guiObj2->subInstance;
	obj->pos_percent = (vec2f){ .5f, 0.0f };
	obj->scale_percent = (vec2f){ .5f, 1.0f };
	obj->scale_px = (vec2i){ -10, -10 };

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


