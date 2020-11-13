#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include <dwmapi.h>
#include "chessGame.h"

#pragma comment(lib, "dwmapi")

Instance* screenUI;

const int default_padding = 0;
PT_COLOR accentColor = { 0 };

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CHESS_GAME* chessGame;

void onRender() {
	PT_ABS_DIM dims = { 0 };
	dims.position = (vec2i){ 0 };
	dims.size = screenSize;

	PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
	dims.sortingType = ZST_GLOBAL; // ui->sortingType;

	render_gui_instance(screenUI, dims);
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

	PT_CREATE_MAIN_WND((vec2i) { 600, 600 }, "PeachTea Chess");

	screenUI = PT_SCREEN_UI_new();

	Instance* backgroundInstance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* backgroundFrame = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundFrame->borderWidth = 0;
	backgroundFrame->size = PT_REL_DIM_new(1, 0, 1, 0);
	backgroundFrame->backgroundTransparency = 0;
	backgroundFrame->backgroundColor = PT_COLOR_fromRGB(100, 100, 100);
	backgroundFrame->visible = 0;
	set_instance_parent(backgroundInstance, screenUI);

	Instance* frameInstance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* boardFrame = (PT_GUI_OBJ*)frameInstance->subInstance;
	boardFrame->visible = FALSE;
	boardFrame->borderWidth = 0;
	boardFrame->size = PT_REL_DIM_new(
		1, -default_padding,
		1, -default_padding
	);
	boardFrame->position = PT_REL_DIM_new(0.5f, 0, 0.5f, 0);
	boardFrame->visible = 1;
	boardFrame->backgroundColor = PT_COLOR_fromRGB(255, 0, 0);
	boardFrame->anchorPosition = (vec2f){ .5f, .5f };

	PT_SIZE_CONSTRAINT_destroy(boardFrame->sizeConstraint);
	boardFrame->sizeConstraint = PT_SIZE_CONSTRAINT_aspect(1.0f, PTSC_LOCK_SMALLEST);
	set_instance_parent(frameInstance, backgroundInstance);

	PT_COLOR board1 = PT_COLOR_lerp(accentColor, PT_COLOR_new(0, 0, 0), .25);
	PT_COLOR board2 = PT_COLOR_fromRGB(200, 200, 200);

	PT_IMAGE dotImage = PT_IMAGE_from_png("assets\\images\\dot.png");

	// create chess board
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Instance* frame = PT_IMAGELABEL_new();

			PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)frame->subInstance;
			imagelabel->image = dotImage;
			imagelabel->imageTint = accentColor;
			imagelabel->imageTintAlpha = 1.0f;
			imagelabel->imageTransparency = 1.0f;

			PT_GUI_OBJ* obj = imagelabel->guiObj;

			obj->reactive = TRUE;
			obj->backgroundColor = (x + y) % 2 == 0 ? board1 : board2;
			obj->activeBackgroundColor = obj->backgroundColor; // PT_COLOR_new(1, 1, 1);
			obj->activeBorderColor = PT_COLOR_new(1, 1, 1);
			obj->activeBackgroundRange = (vec2f){ 40, 250 };
			obj->activeBorderRange = (vec2f){ 25, 70 };
			obj->backgroundTransparency = 0.0f;
			obj->borderWidth = 1;

			obj->position = PT_REL_DIM_new(
				((float)x) / 8.0f, 0,
				((float)y) / 8.0f, 0
			);

			obj->size = PT_REL_DIM_new(
				1.0f / 8.0f, -default_padding,
				1.0f / 8.0f, -default_padding
			);

			obj->borderColor = accentColor;

			char* name = calloc(15, sizeof(char));
			sprintf(name, "%i,%i", x + 1, y + 1); // add to each dimension, origin is at (1, 1); not (0, 0)
			//frame->name = name;

			set_instance_parent(frame, frameInstance);

			Instance* dotFrame = clone_instance(frame);
			dotFrame->name = name;

			PT_IMAGELABEL* dotImage = (PT_IMAGELABEL*)dotFrame->subInstance;
			dotImage->imageTransparency = .5f;
			dotImage->reactive = 1;
			dotImage->activeBackgroundColor = PT_COLOR_lerp(accentColor, PT_COLOR_new(1, 1, 1), .3);
			dotImage->activeBackgroundRange = (vec2f){ 20, 50 };
			dotImage->visible = FALSE;

			PT_GUI_OBJ* dotObj = dotImage->guiObj;
			dotObj->backgroundTransparency = 1.0f;
			dotObj->zIndex = 3;

			set_instance_parent(dotFrame, frameInstance);
		}
	}

	chessGame = create_chess_game(frameInstance);

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


