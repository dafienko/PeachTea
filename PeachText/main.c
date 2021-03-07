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

PT_IMAGE menuImage, arrowImage;
PT_IMAGE sunImage, moonImage;

PT_SCROLLFRAME* editorListScrollframe;
PT_RENDERFRAME* renderFrame;
PT_RENDERFRAME* sideRenderFrame;

int updateRendertree = 0;

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
	if (updateRendertree) {
		PT_SCREEN_UI* ui = (PT_SCREEN_UI*)screenUI->subInstance;
		PT_SCREEN_UI_update_rendertree(ui);
		updateRendertree = 0;
	}

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

void on_new_file(void* args) {
	TEXT_EDITOR_new(backgroundObj->instance, renderFrame, sideRenderFrame, editorListScrollframe);
}

void on_toggle_theme(void* args) {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)args;
	PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)obj->instance->subInstance;

	if (imageLabel->image.texId == moonImage.texId) {
		imageLabel->image = sunImage;
		realize_color_theme(*lightTheme);
	}
	else {
		imageLabel->image = moonImage;
		realize_color_theme(*darkTheme);
	}
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

void on_close() { 

	// iterate through every text editor. If the file is unsaved, prompt the user to save or discard it
	PT_EXPANDABLE_ARRAY editors = get_open_editors();
	for (int i = 0; i < editors.numElements; i++) {
		TEXT_EDITOR* editor = *(TEXT_EDITOR**)PT_EXPANDABLE_ARRAY_get(&editors, i);

		if (!editor->saved) {
			char* question = calloc(200, sizeof(char));
			sprintf(question, "")
			MessageBoxA(NULL, "There")
		}
	}

}


void update_rendertree() {
	updateRendertree = 1;
}

int main(int argc, char** args) {
	PT_CREATE_MAIN_WND((vec2i) { 1100, 800 }, "PeachText");

	sideFrameTransparency = .3f;

#ifndef _DEBUG 
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	darkTheme = calloc(1, sizeof(EDITOR_COLOR_THEME));
	darkTheme->accentColor = accentColor;
	darkTheme->backgroundColor = PT_COLOR_fromHSV(0, 0, 30.0f / 255.0f);
	darkTheme->editColor = accentColor;
	darkTheme->sidebarColor = PT_COLOR_new(0, 0, 0);
	darkTheme->textColor = PT_COLOR_fromHSV(0, 0, .9);
	darkTheme->borderColor = PT_COLOR_fromHSV(0, 0, 1);
	darkTheme->selectedLineColor = PT_COLOR_fromHSV(0, 0, .2f);
	darkTheme->cursorColor = PT_COLOR_fromHSV(0, 0, 1);
	darkTheme->id = 1;

	lightTheme = calloc(1, sizeof(EDITOR_COLOR_THEME));
	lightTheme->accentColor = accentColor;
	lightTheme->backgroundColor = PT_COLOR_fromHSV(0, 0, .95f);
	lightTheme->editColor = accentColor;
	lightTheme->sidebarColor = PT_COLOR_fromHSV(0, 0, .8f);
	lightTheme->textColor = PT_COLOR_fromHSV(0, 0, .04f);
	lightTheme->borderColor = PT_COLOR_fromHSV(0, 0, 0);
	lightTheme->selectedLineColor = PT_COLOR_fromHSV(0, 0, .85f);
	lightTheme->cursorColor = PT_COLOR_fromHSV(0, 0, 0);
	lightTheme->id = 2;

	colorTheme = *darkTheme;

	menuImage = PT_IMAGE_from_png("assets\\images\\menu.png");
	arrowImage = PT_IMAGE_from_png("assets\\images\\arrow.png");
	sunImage = PT_IMAGE_from_png("assets\\images\\sun.png");
	moonImage = PT_IMAGE_from_png("assets\\images\\moon.png");


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
	backgroundObj = (PT_GUI_OBJ*)backgroundInstance->subInstance;
	backgroundObj->backgroundColor = colorTheme.backgroundColor;
	backgroundObj->size = PT_REL_DIM_new(1.0f, 0, 1.0f, 0);
	backgroundObj->zIndex = 0;

	set_instance_parent(backgroundInstance, screenUI);

	// side menu 
	Instance* sideBarInstance = PT_GUI_OBJ_new();
	sideBarInstance->name = create_heap_str("sidebar");
	sideBarObj = (PT_GUI_OBJ*)sideBarInstance->subInstance;
	sideBarObj->size = PT_REL_DIM_new(0, SIDE_MENU_WIDTH, 1.0f, -STATUS_BAR_HEIGHT);
	sideBarObj->anchorPosition = (vec2f){1.0f, 0.0f };
	sideBarObj->position = MENU_CLOSE_POS;
	
	sideBarObj->blurred = 1;
	sideBarObj->blurAlpha = 0.6f;
	sideBarObj->blurRadius = 50;

	sideBarObj->backgroundColor = colorTheme.sidebarColor;

	sideBarObj->borderWidth = 0;
	sideBarObj->reactive = 1;
	sideBarObj->borderColor = PT_COLOR_fromRGB(40, 40, 40);
	sideBarObj->activeBorderColor = PT_COLOR_fromRGB(100, 100, 100);
	sideBarObj->activeBorderRange = (vec2f){ 10, 80 };
	sideBarObj->zIndex = 10;

	TWEEN_CONFIG config = { 0 };
	config.direction = PT_OUT;
	config.duration = .35f;
	config.type = PT_CUBIC;

	openMenuTween = PT_TWEEN_PT_REL_DIM_new(MENU_OPEN_POS, &sideBarObj->position, config);
	closeMenuTween = PT_TWEEN_PT_REL_DIM_new(MENU_CLOSE_POS, &sideBarObj->position, config);

	set_instance_parent(sideBarInstance, backgroundInstance);

	PT_COLOR fringeColor = PT_TWEEN_PT_COLOR(colorTheme.sidebarColor, colorTheme.backgroundColor, sideFrameTransparency, PT_LINEAR, PT_IN);

	Instance* sidebarHeaderInstance = PT_TEXTLABEL_new();
	sidebarHeaderInstance->name = create_heap_str("sidebar header");
	sidebarHeader = (PT_TEXTLABEL*)sidebarHeaderInstance->subInstance;
	sidebarHeader->text = create_heap_str("Files");
	sidebarHeader->textSize = 20;
	sidebarHeader->font = PT_FONT_CONSOLA_B;
	sidebarHeader->textColor = colorTheme.textColor;
	sidebarHeader->horizontalAlignment = PT_H_ALIGNMENT_LEFT;
	sidebarHeader->verticalAlignment = PT_V_ALIGNMENT_CENTER;
	sidebarHeader->fringeColor = colorTheme.sidebarColor;
	PT_GUI_OBJ* sidebarHeaderObj = sidebarHeader->guiObj;
	sidebarHeaderObj->position = PT_REL_DIM_new(0, 16, 0, 0);
	sidebarHeaderObj->size = PT_REL_DIM_new(1.0f, -(12 + SIDE_BAR_WIDTH), 0, 50);
	sidebarHeaderObj->backgroundTransparency = 1;

	set_instance_parent(sidebarHeaderInstance, sideBarInstance);

	// side bar text editor list scrollframe
	editorListScrollframe = create_editor_scrollframe();
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
	menuButton->imageTint = colorTheme.borderColor;
	menuButton->imageTintAlpha = .85f;

	PT_GUI_OBJ* menuButtonObj = menuButton->guiObj;
	menuButtonObj->size = PT_REL_DIM_new(0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2, 0, SIDE_BAR_WIDTH - SIDE_BAR_PADDING * 2);
	menuButtonObj->anchorPosition = (vec2f){ 1, 0 };
	menuButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 0, SIDE_BAR_PADDING);
	
	menuButtonObj->backgroundColor = colorTheme.sidebarColor;
	menuButtonObj->backgroundTransparency = .5f;

	menuButtonObj->reactive = 1;
	menuButtonObj->activeBackgroundRange = sideBarObj->activeBorderRange;
	menuButtonObj->activeBackgroundColor = sideBarObj->activeBorderColor;
	PT_BINDABLE_EVENT_bind(&menuButtonObj->e_obj_activated, on_menu_activated);
	set_instance_parent(menuButtonInstance, sideBarInstance);

	// color theme toggle button
	Instance* themeButtonInstance = clone_instance(menuButtonInstance);
	themeButton = (PT_IMAGELABEL*)themeButtonInstance->subInstance;
	themeButton->image = moonImage;
	themeButton->imageScale = .8f;
	PT_GUI_OBJ* themeObj = themeButton->guiObj;
	themeObj->anchorPosition = (vec2f){ 1, 1 };
	themeObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -4 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&themeObj->e_obj_activated, on_toggle_theme);
	set_instance_parent(themeButtonInstance, sideBarInstance);
	
	// word-wrap toggle button
	Instance* wordWrapInstance = clone_instance(menuButtonInstance);
	wordWrapButton = (PT_IMAGELABEL*)wordWrapInstance->subInstance;
	wordWrapButton->image = PT_IMAGE_from_png("assets\\images\\word_wrap.png");
	wordWrapButton->imageScale = .8f;
	PT_GUI_OBJ* wordWrapButtonObj = wordWrapButton->guiObj;
	wordWrapButtonObj->anchorPosition = (vec2f){ 1, 1 };
	wordWrapButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -3 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&wordWrapButtonObj->e_obj_activated, on_toggle_wordWrap);
	set_instance_parent(wordWrapInstance, sideBarInstance);
	
	// new file button
	Instance* newButtonInstance = clone_instance(menuButtonInstance);
	newFileButton = (PT_IMAGELABEL*)newButtonInstance->subInstance;
	newFileButton->image = PT_IMAGE_from_png("assets\\images\\new.png");;
	newFileButton->imageScale = .8f;
	PT_GUI_OBJ* newObj = newFileButton->guiObj;
	newObj->anchorPosition = (vec2f){ 1, 1 };
	newObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -2 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&newObj->e_obj_activated, on_new_file);
	set_instance_parent(newButtonInstance, sideBarInstance);

	// save-as button
	Instance* saveAsButtonInstance = clone_instance(menuButtonInstance);
	saveAsButton = (PT_IMAGELABEL*)saveAsButtonInstance->subInstance;
	saveAsButton->image = PT_IMAGE_from_png("assets\\images\\saveAs.png");
	saveAsButton->imageScale = .7f;
	PT_GUI_OBJ* saveAsButtonObj = saveAsButton->guiObj;
	saveAsButtonObj->anchorPosition = (vec2f){ 1, 1 };
	saveAsButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, -1 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&saveAsButtonObj->e_obj_activated, on_save_as);
	set_instance_parent(saveAsButtonInstance, sideBarInstance);

	// save button
	Instance* saveButtonInstance = clone_instance(menuButtonInstance);
	saveButton = (PT_IMAGELABEL*)saveButtonInstance->subInstance;
	saveButton->image = PT_IMAGE_from_png("assets\\images\\save.png");
	saveButton->imageScale = .7f;
	PT_GUI_OBJ* saveButtonObj = saveButton->guiObj;
	saveButtonObj->anchorPosition = (vec2f){ 1, 1 };
	saveButtonObj->position = PT_REL_DIM_new(1, -SIDE_BAR_PADDING, 1, 0 * ((SIDE_BAR_WIDTH - 2 * SIDE_BAR_PADDING) + SIDE_BAR_PADDING) - SIDE_BAR_PADDING);
	PT_BINDABLE_EVENT_bind(&saveButtonObj->e_obj_activated, on_save);
	set_instance_parent(saveButtonInstance, sideBarInstance);

	// bottom status bar
	Instance* statusBarInstance = PT_TEXTLABEL_new();
	statusBarInstance->name = create_heap_str("status bar");
	statusBarLabel = (PT_TEXTLABEL*)statusBarInstance->subInstance;
	statusBarLabel->horizontalAlignment = PT_H_ALIGNMENT_RIGHT;
	statusBarLabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;
	statusBarLabel->textColor = colorTheme.textColor;
	statusBarLabel->font = PT_FONT_CONSOLA;
	statusBarLabel->textSize = 12;
	statusBarLabel->text = status;
	statusBarLabel->textTransparency = .3f;
	statusBarLabel->fringeColor = colorTheme.sidebarColor;

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
	renderFrame = (PT_RENDERFRAME*)renderInstance->subInstance;
	renderFrame->clearColor = colorTheme.backgroundColor;
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
	sideRenderFrame = (PT_RENDERFRAME*)sideRenderInstance->subInstance;
	sideRenderFrame->clearColor = colorTheme.sidebarColor;
	PT_GUI_OBJ* sideRenderObj = sideRenderFrame->guiObj;
	sideRenderObj->position = PT_REL_DIM_new(0, 0, 0, 0);
	sideRenderObj->size = PT_REL_DIM_new(0, 100, 1, 0);
	sideRenderObj->zIndex = 4;
	sideRenderObj->backgroundColor = colorTheme.sidebarColor;
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
			TEXT_EDITOR_select(e);
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