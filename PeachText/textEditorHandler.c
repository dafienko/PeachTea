#include "textEditorHandler.h"
#include "PeachTea.h";
#include "textCursor.h"

#include <stdlib.h>
#include <stdio.h>

TEXT_EDITOR* currentTextEditor;
PT_EXPANDABLE_ARRAY* lines;
int keyDownBound = 0;

vec2i get_dir(int key) {
	vec2i dir = { 0 };

	switch (key) {
	case VK_UP:
		dir.y = 1;
		break;
	case VK_DOWN:
		dir.y = -1;
		break;
	case VK_LEFT:
		dir.x = -1;
		break;
	case VK_RIGHT:
		dir.x = 1;
		break;
	}

	return dir;
}

void on_char_typed(void* args) {
	char c = *(char*)args;

	if (c < 32 && c != '\b' && c != '\n' && c != '\t') { // if its not a text-releated visible character (like ESC)
		return;
	}
	
	TEXT_CURSOR* cursor = &currentTextEditor->textCursor;
	int cursorY = cursor->position.y;
	int cloneLineY = cursorY + cursor->cloneLineOffset;

	int startY = min(cursorY, cloneLineY);
	int endY = max(cursorY, cloneLineY);
	for (int y = endY; y >= startY; y--) {
		vec2i cursorPos = cursor->position;

		if (c == '\b') {
			vec2i end = cursorPos;
			vec2i start = (vec2i){ 0 };

			if (end.y > 0 || end.x > 0) { // can't delete anything if cursor is at 0, 0
				start.x = end.x - 1;
				start.y = end.y;

				if (start.x < 0) { // -x line index, go up a line and put x at the end of that line
					TEXT_LINE lastLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, end.y - 1);
					start.y = end.y - 1;
					start.x = lastLine.numChars - 1; // sub 1 so we're before the last line's '\n'
				}

				remove_str_at_cursor(cursor, start, end);
			}
		}
		else {
			vec2i p = (vec2i){ cursor->position.x, y };
			insert_str_at_cursor(cursor, p, &c, 1);
		}
	}	
}

void on_key_down(void* args) {
	if (!currentTextEditor) {
		return; // don't do anything if there is no current text editor
	}

	int key = *(int*)args;
	vec2i dir = get_dir(key);

	int shiftDown = is_key_down(VK_SHIFT);
	if (dir.x != 0 || dir.y != 0) { // if the cursor actually moved
		move_cursor(&currentTextEditor->textCursor, dir, shiftDown, 0);
	}
}

void on_sys_key_down(void* args) {
	if (!currentTextEditor) {
		return;
	}

	int key = *(int*)args;
	vec2i dir = get_dir(key);

	int shiftDown = is_key_down(VK_SHIFT);
	if (dir.x != 0 || dir.y != 0) { // if the cursor actually moved
		move_cursor(&currentTextEditor->textCursor, dir, shiftDown, 1);
	}
}

char get_last_char(TEXT_LINE line) {
	return *(line.str + (line.numChars - 1));
}

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe) {
	TEXT_EDITOR* editor = calloc(1, sizeof(TEXT_EDITOR));

	editor->textLines = calloc(1, sizeof(PT_EXPANDABLE_ARRAY));
	*editor->textLines = PT_EXPANDABLE_ARRAY_new(50, sizeof(TEXT_LINE));

	editor->numTextLabels = 20;
	editor->textlabels = calloc(editor->numTextLabels, sizeof(Instance*));

	editor->textHeight = 20;
	editor->linePadding = 10;
	editor->charWidth = 11;

	for (int i = 0; i < editor->numTextLabels; i++) {
		Instance* textlabelInstance = PT_TEXTLABEL_new();

		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)textlabelInstance->subInstance;
		textlabel->font = PT_FONT_CONSOLA;
		textlabel->textSize = editor->textHeight;
		textlabel->textColor = PT_COLOR_new(1, 1, 1);
		textlabel->horizontalAlignment = PT_H_ALIGNMENT_LEFT;
		textlabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;

		PT_GUI_OBJ* textobj = textlabel->guiObj;
		textobj->size = PT_REL_DIM_new(1.0f, -40, 0, editor->textHeight);
		textobj->anchorPosition = (vec2f){ .5f, 0 };
		textobj->position = PT_REL_DIM_new(.5f, 0, 0, (editor->textHeight + editor->linePadding) * i);
		textobj->backgroundTransparency = 1;
		//float h = ((float)rand() / (float)RAND_MAX) * 360.0f;
		//textobj->backgroundColor = PT_COLOR_fromHSV(h, 1, 1);

		set_instance_parent(textlabelInstance, scrollframe);

		*(editor->textlabels + i) = textlabelInstance;
	}

	// init first text line
	TEXT_LINE firstLine = { 0 };
	firstLine.str = calloc(1, sizeof(char));
	firstLine.numChars = 0;
	firstLine.numCharSpace = 1;
	PT_EXPANDABLE_ARRAY_add(editor->textLines, (void*)&firstLine);

	// init first cursor
	Instance* cursorFrame = PT_GUI_OBJ_new();
	PT_GUI_OBJ* cursorObj = (PT_GUI_OBJ*)cursorFrame->subInstance;
	cursorObj->size = PT_REL_DIM_new(0, 2, 0, editor->textHeight + editor->linePadding);
	cursorObj->zIndex = 5;

	set_instance_parent(cursorFrame, scrollframe);

	TEXT_CURSOR mainCursor = { 0 };
	mainCursor.position = (vec2i){ 0, 0 };
	mainCursor.textArray = editor->textLines;
	mainCursor.cursorFrame = cursorFrame;
	editor->textCursor = mainCursor;

	currentTextEditor = editor;

	if (!keyDownBound) {
		keyDownBound = 1;
		PT_BINDABLE_EVENT_bind(&eOnCharTyped, on_char_typed);
		PT_BINDABLE_EVENT_bind(&eOnKeyPress, on_key_down);
		PT_BINDABLE_EVENT_bind(&eOnSysKeyPress, on_sys_key_down);
	}

	return editor;
}

void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt) {
	for (int i = 0; i < editor->numTextLabels; i++) {
		Instance* instance = *(editor->textlabels + i);
		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)instance->subInstance;

		char* str = NULL;

		if (i < editor->textLines->numElements) {
			TEXT_LINE textline = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(editor->textLines, i);
			str = textline.str;
		}

		textlabel->text = str;
	}

	TEXT_CURSOR* textCursor = &editor->textCursor;
	int cursorY = textCursor->position.y;
	int cloneLineY = cursorY + textCursor->cloneLineOffset;

	int startY = min(cursorY, cloneLineY);
	int endY = max(cursorY, cloneLineY);
	for (int y = startY; y <= endY; y++) {
		vec2i cursorPos = textCursor->position;

		PT_GUI_OBJ* cursorObj = (PT_GUI_OBJ*)textCursor->cursorFrame->subInstance;
		cursorObj->position = PT_REL_DIM_new(
			0, 20 + cursorPos.x * editor->charWidth,
			0, cursorPos.y * (editor->linePadding + editor->textHeight)
		); 
	}
}