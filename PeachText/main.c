#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include "glText.h"

#include "ui.h"
#include "textEditorHandler.h";
#include "textCursor.h"

#include <stdlib.h>
#include <stdio.h>
#include <commdlg.h>
#include <direct.h>
#include <windowsx.h>

Instance* screenUI = NULL;
PT_IMAGELABEL* menuButton;
PT_IMAGE menuImage, arrowImage;

PT_TEXTLABEL* statusBarLabel = NULL;
char* status = NULL;

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
	TEXT_EDITOR* textEditor = get_current_text_editor();

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

void collapse_sidebar() {
	if (menuOpen) {
		PT_TWEEN_play(closeMenuTween);
		menuButton->image = menuImage;
	}
}

void on_toggle_wordWrap(void* args) {
	wrapText = wrapText ? 0 : 1;
}

void on_scroll(void* args) {
	TEXT_EDITOR* textEditor = get_current_text_editor();

	if (is_key_down(VK_LCONTROL)) {
		int d = -*(int*)args;

		if (d > 0) {
			d = 1;
		}
		else {
			d = -1;
		}

		int currentTextHeight = editorTextHeight;
		int textHeight = min(100, max(6, currentTextHeight + d));

		editorTextHeight = textHeight;
	}
}

void on_save_as() {
	TEXT_EDITOR* editor = get_current_text_editor();
	if (!editor) {
		return;
	}

	OPENFILENAMEA ofna = { 0 };
	ofna.lStructSize = sizeof(OPENFILENAMEA);
	ofna.nMaxFile = 512;
	ofna.lpstrFile = calloc(ofna.nMaxFile - 1, sizeof(char));
	ofna.Flags = OFN_EXPLORER;
	ofna.lpstrFilter = "Text File\0*.TXT\0All Files\0*.*\0\0";

	if (GetSaveFileNameA(&ofna)) {
		int totalLen = strlen(ofna.lpstrFile);

		int pathLen = ofna.nFileOffset;
		char* path = calloc(pathLen + 1, sizeof(char)); // add 1 for null terminator
		if (pathLen > 0) {
			memcpy(path, ofna.lpstrFile, pathLen * sizeof(char));
		}
		if (editor->path) {
			free(editor->path);
		}
		editor->path = path;

		int nameLen = (ofna.nFileExtension - ofna.nFileOffset) - 1; // subtract 1 (exclude the '.')
		char* filename = calloc(nameLen + 1, sizeof(char)); 
		if (nameLen > 0) {
			memcpy(filename, ofna.lpstrFile + ofna.nFileOffset, nameLen * sizeof(char));
		}
		if (editor->filename) {
			free(editor->filename);
		}
		editor->filename = filename;

		int extensionLen = totalLen - ofna.nFileExtension;
		char* extension = calloc(extensionLen + 1, sizeof(char));
		if (extensionLen > 0) {
			memcpy(extension, ofna.lpstrFile + ofna.nFileExtension, extensionLen * sizeof(char));
		}
		if (editor->extension) {
			free(editor->extension);
		}
		editor->extension = extension;

		TEXT_EDITOR_save(editor);
		_chdir(initWorkingDir);

	}


	free(ofna.lpstrFile);
}

void on_save() {
	TEXT_EDITOR* editor = get_current_text_editor();
	if (!editor) {
		return;
	}

	if (editor->path) {
		TEXT_EDITOR_save(editor);
	}
	else {
		on_save_as();
	}
}

void main_on_command(void* arg) {
	PT_COMMAND command = *(PT_COMMAND*)arg;

	switch (command) {
	case PT_SAVE:
		on_save();
		break;
	case PT_SAVE_AS:
		on_save_as();
		break;
	}
}

void update_rendertree() {
	PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
	PT_SCREEN_UI_update_rendertree(ui);
}

int main(int argc, char** args) {
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

	// side menu 
	Instance* sideBarInstance = PT_GUI_OBJ_new();
	sideBarInstance->name = create_heap_str("sidebar");
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

	Instance* sidebarHeaderInstance = PT_TEXTLABEL_new();
	sidebarHeaderInstance->name = create_heap_str("sidebar header");
	PT_TEXTLABEL* sidebarHeader = (PT_TEXTLABEL*)sidebarHeaderInstance->subInstance;
	sidebarHeader->text = create_heap_str("Files");
	sidebarHeader->textSize = 20;
	sidebarHeader->font = PT_FONT_CONSOLA_B;
	sidebarHeader->textColor = PT_COLOR_fromHSV(0, 0, 1);
	sidebarHeader->horizontalAlignment = PT_H_ALIGNMENT_LEFT;
	sidebarHeader->verticalAlignment = PT_V_ALIGNMENT_CENTER;
	PT_GUI_OBJ* sidebarHeaderObj = sidebarHeader->guiObj;
	sidebarHeaderObj->position = PT_REL_DIM_new(0, 16, 0, 0);
	sidebarHeaderObj->size = PT_REL_DIM_new(1.0f, -(12 + SIDE_BAR_WIDTH), 0, 50);
	sidebarHeaderObj->backgroundTransparency = 1;

	set_instance_parent(sidebarHeaderInstance, sideBarInstance);

	// side bar text editor list scrollframe
	PT_SCROLLFRAME* editorListScrollframe = create_editor_scrollframe();
	editorListScrollframe->instance->name = create_heap_str("editor list scrollframe");
	editorListScrollframe->scrollBarThickness = 5;

	PT_GUI_OBJ* editorListObj = editorListScrollframe->guiObj;
	editorListObj->position = PT_REL_DIM_new(0, 12, 0, 50);
	editorListObj->anchorPosition = (vec2f){ 0, 0 };
	editorListObj->size = PT_REL_DIM_new(1.0f, -(12 + SIDE_BAR_WIDTH), 1.0f, -62);
	editorListObj->backgroundTransparency = 1.0f;
	editorListObj->borderWidth = 0;
	editorListObj->clipDescendants = 1;

	set_instance_parent(editorListScrollframe->instance, sideBarInstance);

	// side menu collapse/expand button
	Instance* menuButtonInstance = PT_IMAGELABEL_new();
	menuButtonInstance->name = create_heap_str("menu button");
	menuButton = (PT_IMAGELABEL*)menuButtonInstance->subInstance;
	menuButton->image = menuImage;
	menuButton->imageTint = PT_COLOR_fromHSV(0, 0, 1);
	menuButton->imageTintAlpha = 1.0f;

	PT_GUI_OBJ* menuButtonObj = menuButton->guiObj;
	menuButtonObj->size = PT_REL_DIM_new(0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2, 0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2);
	menuButtonObj->anchorPosition = (vec2f){ 1, 0 };
	menuButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 0, SIDE_BAR_PADDING);
	
	menuButtonObj->backgroundColor = PT_COLOR_fromHSV(0, 0, 0);
	menuButtonObj->backgroundTransparency = .5f;

	menuButtonObj->reactive = 1;
	menuButtonObj->activeBackgroundRange = sideBarObj->activeBorderRange;
	menuButtonObj->activeBackgroundColor = sideBarObj->activeBorderColor;

	PT_BINDABLE_EVENT_bind(&menuButtonObj->e_obj_activated, on_menu_activated);

	set_instance_parent(menuButtonInstance, sideBarInstance);

	// word-wrap toggle button
	Instance* wordWrapInstance = clone_instance(menuButtonInstance);
	PT_IMAGELABEL* wordWrapButton = (PT_IMAGELABEL*)wordWrapInstance->subInstance;
	wordWrapButton->imageTintAlpha = 0;
	wordWrapButton->image = PT_IMAGE_from_png("assets\\images\\word_wrap.png");
	wordWrapButton->imageScale = .8f;
	PT_GUI_OBJ* wordWrapButtonObj = wordWrapButton->guiObj;
	wordWrapButtonObj->anchorPosition = (vec2f){ 1, 1 };
	wordWrapButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -2 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&wordWrapButtonObj->e_obj_activated, on_toggle_wordWrap);
	set_instance_parent(wordWrapInstance, sideBarInstance);

	// save button
	Instance* saveButtonInstance = clone_instance(menuButtonInstance);
	PT_IMAGELABEL* saveButton = (PT_IMAGELABEL*)saveButtonInstance->subInstance;
	saveButton->image = PT_IMAGE_from_png("assets\\images\\save.png");
	saveButton->imageScale = .7f;
	PT_GUI_OBJ* saveButtonObj = saveButton->guiObj;
	saveButtonObj->anchorPosition = (vec2f){ 1, 1 };
	saveButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, 0 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&saveButtonObj->e_obj_activated, on_save);
	set_instance_parent(saveButtonInstance, sideBarInstance);
	
	// save-as button
	Instance* saveAsButtonInstance = clone_instance(menuButtonInstance);
	PT_IMAGELABEL* saveAsButton = (PT_IMAGELABEL*)saveAsButtonInstance->subInstance;
	saveAsButton->image = PT_IMAGE_from_png("assets\\images\\saveAs.png");
	saveAsButton->imageScale = .7f;
	PT_GUI_OBJ* saveAsButtonObj = saveAsButton->guiObj;
	saveAsButtonObj->anchorPosition = (vec2f){ 1, 1 };
	saveAsButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -1 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&saveAsButtonObj->e_obj_activated, on_save_as);
	set_instance_parent(saveAsButtonInstance, sideBarInstance);

	// bottom status bar
	Instance* statusBarInstance = PT_TEXTLABEL_new();
	statusBarInstance->name = create_heap_str("status bar");
	statusBarLabel = (PT_TEXTLABEL*)statusBarInstance->subInstance;
	statusBarLabel->horizontalAlignment = PT_H_ALIGNMENT_RIGHT;
	statusBarLabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;
	statusBarLabel->textColor = PT_COLOR_fromHSV(0, 0, .85f);
	statusBarLabel->font = PT_FONT_CONSOLA;
	statusBarLabel->textSize = 12;
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

	EDITOR_STATE_CONFIG editorConfig = load_editor_state_config();
	realize_editor_state_config(backgroundInstance, renderFrame, sideRenderFrame, editorListScrollframe, editorConfig);
	free_editor_state_config(&editorConfig);

	if (argc <= 1) {
		TEXT_EDITOR* e = TEXT_EDITOR_new(backgroundInstance, renderFrame, sideRenderFrame, editorListScrollframe);
	}
	else {
		for (int i = 1; i < argc; i++) {
			char* filename = *(args + i);

			TEXT_EDITOR* e = TEXT_EDITOR_from_file(backgroundInstance, renderFrame, sideRenderFrame, editorListScrollframe, filename);
		}
	}
	

	update_rendertree();

	PT_BINDABLE_EVENT_bind(&e_wheelUp, on_scroll);
	PT_BINDABLE_EVENT_bind(&e_wheelDown, on_scroll);
	PT_BINDABLE_EVENT_bind(&eOnCommand, main_on_command);

	PT_RUN(onUpdate, onRender);

	editorConfig = get_editor_state_config();
	save_state_config(editorConfig);
	free_editor_state_config(&editorConfig);
	
	free(status);
}