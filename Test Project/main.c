#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi")

Instance* screenUI;

const int default_padding = 5;
vec3i accentColor = { 0 };

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void onRender() {
	PT_GUI_DIMS dims = { 0 };
	dims.absolutePos = (vec2i){ 0 };
	dims.absoluteSize = screenSize;

	render_gui_instance(screenUI, dims);
}

void setup_default_gui_obj(PT_GUI_OBJ* obj) {
	obj->border_color = (vec3i){ 255, 255, 255 };
	obj->background_color = (vec3i){ 80, 80, 80 };
	obj->transparency = .66;

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

	screenUI = PT_SCREEN_UI_new(NULL);

	Instance* backgroundInstance = create_default_frame(screenUI);
	PT_GUI_OBJ* backgroundFrame = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundFrame->borderWidth = 0;
	backgroundFrame->scale_percent = (vec2f){ 1, 1 };
	backgroundFrame->transparency = 0;
	backgroundFrame->background_color = (vec3i){ 100, 100, 100 };

	Instance* frameInstance = create_default_frame(screenUI);
	PT_GUI_OBJ* frame = (PT_GUI_OBJ*)frameInstance->subInstance;
	frame->visible = FALSE;
	frame->borderWidth = 0;
	frame->scale_percent = (vec2f){ 1, 1 };
	frame->scale_px = (vec2i){ -default_padding, -default_padding };
	frame->pos_px = (vec2i){ default_padding, default_padding };

	int numRows = 5;
	int numColumns = 6;

	
	for (int x = 0; x < numColumns; x++) {
		for (int y = 0; y < numRows; y++) {
			///*
			Instance* button = create_default_textlabel(frameInstance);

			PT_TEXTLABEL* tl = (PT_TEXTLABEL*)button->subInstance;

			PT_GUI_OBJ* obj = (PT_GUI_OBJ*)tl->guiObj;

			obj->pos_px = (vec2i){ 0, 0};
			obj->pos_percent = (vec2f){ ((float)x) / numColumns, ((float)y) / numRows };
			obj->scale_percent = (vec2f){ 1.0f / numColumns, 1.0f / numRows };
			obj->scale_px = (vec2i){ -default_padding, -default_padding };

			tl->font = PT_FONT_COMIC;
			tl->textSize = 20;
			tl->horizontalAlignment = PT_H_ALIGNMENT_CENTER;
			tl->verticalAlignment = PT_V_ALIGNMENT_CENTER;

			tl->text = calloc(10, sizeof(char));
			sprintf(tl->text, "%i, %i", x, y);
			//*/
		}
	}
	

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


