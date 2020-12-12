#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"
#include "chessGame.h"


Instance* screenUI;

const int default_padding = 0;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CHESS_GAME* chessGame;
CHESS_GAME* chessGame;

void onRender() {
	PT_ABS_DIM dims = { 0 };
	dims.position = (vec2i){ 0 };
	dims.size = screenSize;

	PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
	dims.sortingType = ZST_GLOBAL; // ui->sortingType;

	render_gui_instance(screenUI, dims);
}

int main() {
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

	chessGame = create_chess_game(frameInstance);

	int exitCode = PT_RUN(onRender);

	return exitCode;
}


