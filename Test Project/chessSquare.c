#include "chessSquare.h"

int chessSquareInitialized = 0;
PT_IMAGE dotImage;
PT_COLOR board1;
PT_COLOR board2;

CHESS_SQUARE CHESS_SQUARE_new(vec2i pos, Instance* boardFrame) {
	CHESS_SQUARE chessSquare = { 0 };
	
	if (!chessSquareInitialized) {
		board1 = PT_COLOR_lerp(accentColor, PT_COLOR_new(0, 0, 0), .25);
		board2 = PT_COLOR_fromRGB(200, 200, 200);

		dotImage = PT_IMAGE_from_png("assets\\images\\dot.png");
	}

	Instance* frame = PT_IMAGELABEL_new();

	PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)frame->subInstance;
	imagelabel->image = dotImage;
	imagelabel->imageTint = accentColor;
	imagelabel->imageTintAlpha = 1.0f;
	imagelabel->imageTransparency = 1.0f;

	PT_GUI_OBJ* obj = imagelabel->guiObj;

	obj->reactive = TRUE;
	obj->backgroundColor = (pos.x + pos.y) % 2 == 0 ? board1 : board2;
	obj->activeBackgroundColor = obj->backgroundColor; // PT_COLOR_new(1, 1, 1);
	obj->activeBorderColor = PT_COLOR_new(1, 1, 1);
	obj->activeBackgroundRange = (vec2f){ 40, 250 };
	obj->activeBorderRange = (vec2f){ 25, 70 };
	obj->backgroundTransparency = 0.0f;
	obj->borderWidth = 1;

	obj->position = PT_REL_DIM_new(
		((float)pos.x - 1.0f) / 8.0f, 0,
		((float)pos.y - 1.0f) / 8.0f, 0
	);

	obj->size = PT_REL_DIM_new(
		1.0f / 8.0f, 0,
		1.0f / 8.0f, 0
	);

	obj->borderColor = accentColor;

	set_instance_parent(frame, boardFrame);

	Instance* dotFrame = clone_instance(frame);

	PT_IMAGELABEL* dotImage = (PT_IMAGELABEL*)dotFrame->subInstance;
	dotImage->imageTransparency = 0.1f;
	dotImage->reactive = 1;
	dotImage->activeBackgroundColor = PT_COLOR_lerp(accentColor, PT_COLOR_new(1, 1, 1), .8);
	dotImage->activeBackgroundRange = (vec2f){ 20, 150 };
	dotImage->visible = FALSE;

	PT_GUI_OBJ* dotObj = dotImage->guiObj;
	dotObj->backgroundTransparency = 1.0f;
	dotObj->zIndex = 3;

	set_instance_parent(dotFrame, boardFrame);

	chessSquare.position = pos;
	chessSquare.squareDot = dotImage;
	chessSquare.squareFrame = imagelabel;

	return chessSquare;
}