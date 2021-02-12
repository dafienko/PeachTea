#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include "glText.h"

#include "textEditorHandler.h";
#include "textCursor.h"

#include <stdlib.h>
#include <stdio.h>

Instance* screenUI = NULL;
TEXT_EDITOR* textEditor = NULL;

PT_IMAGE menuImage, arrowImage;

PT_TEXTLABEL* statusBarLabel = NULL;
char* status = NULL;

const int STATUS_BAR_HEIGHT = 20;

const int SIDE_BAR_WIDTH = 40;
const int SIDE_BAR_PADDING = 3;

const int SIDE_MENU_WIDTH = 320;

void onRender() {
	PT_SCREEN_UI_render(screenUI->subInstance);
	
	int lineThickness = 29;
	char_set* cs = get_char_set(PT_FONT_CONSOLA, 24);
}

int frames = 0;
int fps = 0;
int cpm = 0;
int lastTimeIndex = 0;
float fpsUpdateInterval = 1.0f;
void onUpdate(float dt) {
	float time = PT_TIME_get();
	frames++;
	int tIndex = time / fpsUpdateInterval;
	if (tIndex > lastTimeIndex) {
		fps = frames / fpsUpdateInterval;
		frames = 0;

		int* pCharsTyped = get_charsTyped();
		cpm = (*pCharsTyped * 60) / fpsUpdateInterval;
		*pCharsTyped = 0;

		lastTimeIndex = tIndex;
	}

	//*
	TEXT_EDITOR_update(textEditor, dt);

	vec2i cursorPosition = textEditor->textCursor.position;

	const char* s = "      ";
	memset(status, 0, 200 * sizeof(char));
	sprintf(
		status, 
		"fps: %i%scpm: %i%sln: %i%scol: %i%s", 
		fps, s,
		cpm, s,
		cursorPosition.y + 1, s,
		cursorPosition.x + 1, s
	);
	//*/
}

int menuOpen = 0;
PT_REL_DIM MENU_OPEN_POS;
PT_REL_DIM MENU_CLOSE_POS;
PT_TWEEN* openMenuTween;
PT_TWEEN* closeMenuTween;
void on_menu_activated(void* args) {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)args;
	PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)obj->instance->subInstance;

	menuOpen = !menuOpen;

	if (menuOpen) {
		PT_TWEEN_play(openMenuTween);
		imageLabel->image = arrowImage;
	}
	else {
		PT_TWEEN_play(closeMenuTween);
		imageLabel->image = menuImage;
	}
}

void on_toggle_wordWrap(void* args) {
	textEditor->wrapText = textEditor->wrapText ? 0 : 1;
}

																									
int main(int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		printf("%i: \"%s\"\n\n", i, *(args + i));
	}

	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PeachText");

#ifndef _DEBUG 
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	menuImage = PT_IMAGE_from_png("assets\\images\\menu.png");
	arrowImage = PT_IMAGE_from_png("assets\\images\\arrow.png");

	status = calloc(200, sizeof(char));

	MENU_OPEN_POS = PT_REL_DIM_new(0, SIDE_MENU_WIDTH, 0, 0);
	MENU_CLOSE_POS = PT_REL_DIM_new(0, SIDE_BAR_WIDTH, 0, 0);

	//*
	screenUI = PT_SCREEN_UI_new();
	screenUI->name = create_heap_str("screenUI");
	PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
	ui->sortingType = ZST_SIBLING;

	// main background frame
	Instance* backgroundInstance = PT_GUI_OBJ_new();
	backgroundInstance->name = create_heap_str("backgroundFrame");
	PT_GUI_OBJ* backgroundObj = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundObj->backgroundColor = PT_COLOR_fromHSV(0, 0, 30.0f/255.0f);
	backgroundObj->size = PT_REL_DIM_new(1.0f, 0, 1.0f, 0);
	backgroundObj->zIndex = 0;

	set_instance_parent(backgroundInstance, screenUI);



	// main text scrollframe 
	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	scrollFrameInstance->name = create_heap_str("scrollframe");
	PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	scrollframe->canvasSize = PT_REL_DIM_new(0.0f, 0, 0.0f, 1000);
	scrollframe->scrollBarThickness = 12;

	PT_GUI_OBJ* vTrack = scrollframe->vscrollTrack;
	vTrack->backgroundTransparency = 1;
	PT_GUI_OBJ* hTrack = scrollframe->hscrollTrack;
	hTrack->backgroundTransparency = vTrack->backgroundTransparency;

	PT_GUI_OBJ* vBar = scrollframe->vscrollBar;
	vBar->backgroundColor = PT_COLOR_fromHSV(0, 0, .8f);
	vBar->backgroundTransparency = 0.15f;
	vBar->blurred = 1;
	vBar->blurAlpha = .5f;
	vBar->blurRadius = 15;
	vBar->reactive = 1;
	vBar->activeBackgroundColor = PT_COLOR_new(1, 1, 1);
	vBar->activeBackgroundRange = (vec2f){ 10, 200 };
	PT_GUI_OBJ* hBar = scrollframe->hscrollBar;
	hBar->backgroundColor = vBar->backgroundColor;
	hBar->backgroundTransparency = vBar->backgroundTransparency;
	hBar->blurred = vBar->blurred;
	hBar->blurAlpha = vBar->blurAlpha;
	hBar->blurRadius = vBar->blurRadius;
	hBar->reactive = vBar->reactive;
	hBar->activeBackgroundColor = vBar->activeBackgroundColor;
	hBar->activeBackgroundRange = vBar->activeBackgroundRange;

	PT_GUI_OBJ* scrollObj = scrollframe->guiObj;
	scrollObj->backgroundColor = backgroundObj->backgroundColor;
	scrollObj->backgroundTransparency = 1;
	scrollObj->position = PT_REL_DIM_new(0, SIDE_BAR_WIDTH, 0, 0);
	scrollObj->size = PT_REL_DIM_new(1.0f, -SIDE_BAR_WIDTH, 1.0f, -STATUS_BAR_HEIGHT);
	scrollObj->zIndex = 5;
	scrollObj->clipDescendants = 1;
	//scrollObj->processEvents = 0;

	set_instance_parent(scrollFrameInstance, backgroundInstance);

	// side menu 
	Instance* sideBarInstance = PT_GUI_OBJ_new();
	sideBarInstance->name = create_heap_str("side bar");
	PT_GUI_OBJ* sideBarObj = (PT_GUI_OBJ*)sideBarInstance->subInstance;
	sideBarObj->size = PT_REL_DIM_new(0, SIDE_MENU_WIDTH, 1.0f, -STATUS_BAR_HEIGHT);
	sideBarObj->anchorPosition = (vec2f){1.0f, 0.0f };
	sideBarObj->position = MENU_CLOSE_POS;
	
	sideBarObj->blurred = 1;
	sideBarObj->blurAlpha = 0.6f;
	sideBarObj->blurRadius = 50;

	sideBarObj->backgroundColor = PT_COLOR_fromRGB(0, 0, 0);
	//sideBarObj->backgroundColor = accentColor;

	sideBarObj->borderWidth = 0;
	sideBarObj->reactive = 1;
	sideBarObj->borderColor = PT_COLOR_fromRGB(40, 40, 40);
	sideBarObj->activeBorderColor = PT_COLOR_fromRGB(100, 100, 100);
	//sideBarObj->activeBorderColor = PT_COLOR_fromHSV(0, 0, .95f);
	sideBarObj->activeBorderRange = (vec2f){ 10, 80 };
	sideBarObj->zIndex = 10;

	TWEEN_CONFIG config = { 0 };
	config.direction = PT_OUT;
	config.duration = .35f;
	config.type = PT_CUBIC;

	openMenuTween = PT_TWEEN_PT_REL_DIM_new(MENU_OPEN_POS, &sideBarObj->position, config);
	closeMenuTween = PT_TWEEN_PT_REL_DIM_new(MENU_CLOSE_POS, &sideBarObj->position, config);

	set_instance_parent(sideBarInstance, backgroundInstance);

	// side menu collapse/expand button
	Instance* menuButtonInstance = PT_IMAGELABEL_new();
	menuButtonInstance->name = create_heap_str("menu button");
	PT_IMAGELABEL* menuButton = (PT_IMAGELABEL*)menuButtonInstance->subInstance;
	menuButton->image = menuImage;

	PT_GUI_OBJ* menuButtonObj = menuButton->guiObj;
	menuButtonObj->size = PT_REL_DIM_new(0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2, 0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2);
	menuButtonObj->anchorPosition = (vec2f){ 1, 0 };
	menuButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 0, SIDE_BAR_PADDING);
	
	menuButtonObj->backgroundColor = PT_COLOR_new(1, 1, 1);
	menuButtonObj->backgroundTransparency = 1;

	menuButtonObj->reactive = 1;
	menuButtonObj->borderWidth = 1;
	menuButtonObj->borderColor = sideBarObj->borderColor;
	menuButtonObj->activeBorderRange = sideBarObj->activeBorderRange;
	menuButtonObj->activeBorderColor = sideBarObj->activeBorderColor;

	PT_BINDABLE_EVENT_bind(&menuButtonObj->e_obj_activated, on_menu_activated);

	set_instance_parent(menuButtonInstance, sideBarInstance);

	// word-wrap toggle button
	Instance* wordWrapInstance = PT_IMAGELABEL_new();
	wordWrapInstance->name = create_heap_str("menu button");
	PT_IMAGELABEL* wordWrapButton = (PT_IMAGELABEL*)wordWrapInstance->subInstance;
	wordWrapButton->image = PT_IMAGE_from_png("assets\\images\\word_wrap.png");

	PT_GUI_OBJ* wordWrapButtonObj = wordWrapButton->guiObj;
	wordWrapButtonObj->size = PT_REL_DIM_new(0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2, 0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2);
	wordWrapButtonObj->anchorPosition = (vec2f){ 1, 0 };
	wordWrapButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 0, 1 * (SIDE_BAR_WIDTH + SIDE_BAR_PADDING));

	wordWrapButtonObj->backgroundColor = PT_COLOR_new(1, 1, 1);
	wordWrapButtonObj->backgroundTransparency = 1;

	wordWrapButtonObj->reactive = 1;
	wordWrapButtonObj->borderWidth = 1;
	wordWrapButtonObj->borderColor = sideBarObj->borderColor;
	wordWrapButtonObj->activeBorderRange = sideBarObj->activeBorderRange;
	wordWrapButtonObj->activeBorderColor = sideBarObj->activeBorderColor;

	PT_BINDABLE_EVENT_bind(&wordWrapButtonObj->e_obj_activated, on_toggle_wordWrap);

	set_instance_parent(wordWrapInstance, sideBarInstance);

	// bottom status bar
	Instance* statusBarInstance = PT_TEXTLABEL_new();
	statusBarInstance->name = create_heap_str("status bar");
	statusBarLabel = (PT_TEXTLABEL*)statusBarInstance->subInstance;
	statusBarLabel->horizontalAlignment = PT_H_ALIGNMENT_RIGHT;
	statusBarLabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;
	statusBarLabel->textColor = PT_COLOR_fromHSV(0, 0, .85f);
	statusBarLabel->font = PT_FONT_CONSOLA_B;
	statusBarLabel->textSize = 15;
	statusBarLabel->text = status;
	statusBarLabel->textTransparency = .1f;

	PT_GUI_OBJ* statusBarObj = statusBarLabel->guiObj;
	statusBarObj->size = PT_REL_DIM_new(1, 0, 0, STATUS_BAR_HEIGHT);
	statusBarObj->anchorPosition = (vec2f){ 1, 1 };
	statusBarObj->position = PT_REL_DIM_new(1, 0, 1, 0);
	statusBarObj->zIndex = 10;
	statusBarObj->backgroundColor = sideBarObj->backgroundColor;
	statusBarObj->backgroundTransparency = sideBarObj->backgroundTransparency;

	statusBarObj->blurred = sideBarObj->blurred;
	statusBarObj->blurAlpha = sideBarObj->blurAlpha;
	statusBarObj->blurRadius = sideBarObj->blurRadius;

	statusBarObj->reactive = 1;
	statusBarObj->borderWidth = sideBarObj->borderWidth;
	statusBarObj->borderColor = sideBarObj->borderColor;
	statusBarObj->activeBorderRange = sideBarObj->activeBorderRange;
	statusBarObj->activeBorderColor = sideBarObj->activeBorderColor;

	set_instance_parent(statusBarInstance, backgroundInstance);

	// text editor renderframe
	Instance* renderInstance = PT_RENDERFRAME_new();
	renderInstance->name = create_heap_str("render frame");
	PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)renderInstance->subInstance;
	PT_GUI_OBJ* renderObj = renderFrame->guiObj;
	renderObj->position = PT_REL_DIM_new(0, 0, 0, 0);
	renderObj->size = PT_REL_DIM_new(1, 0, 1, 0);
	//renderObj->size = PT_REL_DIM_new(0, 500, 0, 500);
	renderObj->zIndex = 2;
	renderObj->backgroundTransparency = 1;

	set_instance_parent(renderInstance, backgroundInstance);

	// text editor line number renderframe 
	Instance* sideRenderInstance = PT_RENDERFRAME_new();
	sideRenderInstance->name = create_heap_str("side render frame");
	PT_RENDERFRAME* sideRenderFrame = (PT_RENDERFRAME*)sideRenderInstance->subInstance;
	PT_GUI_OBJ* sideRenderObj = sideRenderFrame->guiObj;
	sideRenderObj->position = PT_REL_DIM_new(0, 0, 0, 0);
	sideRenderObj->size = PT_REL_DIM_new(0, 100, 1, 0);
	sideRenderObj->zIndex = 4;
	sideRenderObj->backgroundColor = PT_COLOR_fromRGB(0, 0, 255);
	sideRenderObj->backgroundTransparency = 1;

	set_instance_parent(sideRenderInstance, backgroundInstance);

	textEditor = TEXT_EDITOR_from_file(scrollFrameInstance, renderFrame, sideRenderFrame, "shaders\\core\\blur.fs");
	//*/

	///*
	//if (argc <= 1) {
		//textEditor = TEXT_EDITOR_new(scrollFrameInstance, renderFrame, sideRenderFrame);
	//}
	//else {
		//textEditor = TEXT_EDITOR_from_file(scrollFrameInstance, renderFrame, *(args + 1));
	//}
	//*/

	PT_SCREEN_UI_update_rendertree(ui);

	PT_RUN(onUpdate, onRender);
	//PT_RUN(NULL, onRender);

	free(status);
}