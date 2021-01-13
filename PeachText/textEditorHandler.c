#define _CRT_SECURE_NO_WARNINGS

#include "textEditorHandler.h"
#include "PeachTea.h";
#include "textCursor.h"
#include "textEditorRenderer.h"

#include <stdlib.h>
#include <stdio.h>

TEXT_EDITOR* currentTextEditor = NULL;
PT_EXPANDABLE_ARRAY* lines = NULL;
int keyDownBound = 0;

int charsTyped = 0;

char get_last_char(TEXT_LINE line) {
	return *(line.str + (line.numChars - 1));
}

int* get_charsTyped() {
	return &charsTyped;
}

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
			if (vector_equal_2i(cursor->selectTo, cursor->position)) { // if nothing is selected
				vec2i end = cursorPos;
				vec3i startData = calculate_text_position(cursor->textArray, cursor->position, (vec2i) { -1, 0 }, cursor->targetX);
				vec2i start = (vec2i){ startData.x, startData.y };

				if (!vector_equal_2i(end, start)) { // if there is actually something to be deleted
					remove_str_at_cursor(cursor, start, end);
				}
			}
			else {
				delete_cursor_selection(cursor);
			}
		}
		else if (c < 127) {
			charsTyped++;
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
	else if (key == VK_DELETE) {
		TEXT_CURSOR* cursor = &currentTextEditor->textCursor;

		if (vector_equal_2i(cursor->position, cursor->selectTo)) { // nothing is selected
			vec3i nextPosData = calculate_text_position(cursor->textArray, cursor->position, (vec2i) { 1, 0 }, cursor->targetX);
			vec2i nextPos = (vec2i){ nextPosData.x, nextPosData.y };

			if (!vector_equal_2i(cursor->position, nextPos)) {
				remove_str_at_cursor(cursor, cursor->position, nextPos);
			}
		}
		else {
			delete_cursor_selection(cursor);
		}
	}
	else if (key == VK_INSERT) {
		currentTextEditor->textCursor.insert = !currentTextEditor->textCursor.insert;
	}
}

void copy(TEXT_CURSOR* cursor) {
	if (OpenClipboard(PT_GET_MAIN_HWND())) {
		EmptyClipboard();

		// get selection str data
		char* selection;
		int selectionLength;
		get_cursor_selection(cursor, &selection, &selectionLength);

		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, selectionLength);

		char* pchData;
		pchData = (char*)GlobalLock(hClipboardData);

		memcpy(pchData, selection, selectionLength * sizeof(char));
		free(selection); // we don't need our copy of selection any more, windows has it now

		GlobalUnlock(hClipboardData);

		SetClipboardData(CF_TEXT, hClipboardData);

		CloseClipboard();
	}
}

// https://www.codeproject.com/Articles/2242/Using-the-Clipboard-Part-I-Transferring-Simple-Tex
void on_command(void* args) {
	int command = *(int*)args;

	if (currentTextEditor) {
		TEXT_CURSOR* cursor = &currentTextEditor->textCursor;

		switch (command) {
		case PT_COPY:
			;
			if (!vector_equal_2i(cursor->selectTo, cursor->position)) {
				copy(cursor);
			}
			break;
		case PT_CUT:
			if (!vector_equal_2i(cursor->selectTo, cursor->position)) {
				copy(cursor);

				vec2i start, end;
				get_cursor_selection_bounds(*cursor, &start, &end);
				remove_str_at_cursor(cursor, start, end);
			}
			break;
		case PT_PASTE:
			delete_cursor_selection(cursor);

			if (OpenClipboard(PT_GET_MAIN_HWND()))
			{
				HANDLE hClipboardData = GetClipboardData(CF_TEXT);

				if (hClipboardData) {
					char* pchData = (char*)GlobalLock(hClipboardData);
					int clipboardLen = strlen(pchData);

					insert_str_at_cursor(cursor, cursor->position, pchData, clipboardLen);

					GlobalUnlock(hClipboardData);
				}

				CloseClipboard();
			}
			break;
		case PT_SELECTALL:
			;
			TEXT_LINE lastLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, cursor->textArray->numElements - 1);
			
			cursor->selectTo = (vec2i){ 0, 0 };
			cursor->position = (vec2i){ lastLine.numChars, cursor->textArray->numElements - 1 };

			break;
		}
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

void on_render_frame_render(PT_RENDERFRAME* renderFrame) {
	if (currentTextEditor && currentTextEditor->renderFrame == renderFrame) {
		render_text_editor(*currentTextEditor);
	}
}

int TEXT_EDITOR_get_margin(TEXT_EDITOR* editor) {
	return (int)(editor->charWidth * 7.0f);
}

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframeInstance, PT_RENDERFRAME* renderFrame) {
	if (!keyDownBound) {
		keyDownBound = 1;
		PT_BINDABLE_EVENT_bind(&eOnCharTyped, on_char_typed);
		PT_BINDABLE_EVENT_bind(&eOnKeyPress, on_key_down);
		PT_BINDABLE_EVENT_bind(&eOnSysKeyPress, on_sys_key_down);
		PT_BINDABLE_EVENT_bind(&eOnCommand, on_command);
	}
	
	TEXT_EDITOR* editor = calloc(1, sizeof(TEXT_EDITOR));

	editor->textLines = calloc(1, sizeof(PT_EXPANDABLE_ARRAY));
	*editor->textLines = PT_EXPANDABLE_ARRAY_new(5, sizeof(TEXT_LINE));

	editor->textHeight = 18;
	editor->linePadding = editor->textHeight / 2;

	editor->charSet = get_char_set(PT_FONT_CONSOLA_B, editor->textHeight);

	editor->charWidth = get_text_width(editor->charSet, "M", 1);

	editor->renderFrame = renderFrame;
	editor->scrollFrame = (PT_SCROLLFRAME*)scrollframeInstance->subInstance;


	renderFrame->render = on_render_frame_render;

	// init first text line
	TEXT_LINE firstLine = { 0 };
	firstLine.str = calloc(1, sizeof(char));
	firstLine.numChars = 0;
	firstLine.numCharSpace = 1;
	PT_EXPANDABLE_ARRAY_add(editor->textLines, (void*)&firstLine);

	TEXT_CURSOR mainCursor = TEXT_CURSOR_new(editor);
	editor->textCursor = mainCursor;

	currentTextEditor = editor;

	return editor;
}

TEXT_EDITOR* TEXT_EDITOR_from_file(Instance* scrollframe, PT_RENDERFRAME* renderFrame, const char* filename) {
	TEXT_EDITOR* editor = TEXT_EDITOR_new(scrollframe, renderFrame);
	
	FILE* file = fopen(filename, "rb+");
	if (!file) {
		int len = strlen(filename);
		wchar_t* wfilename = calloc(len + 1, sizeof(wchar_t));
		mbstowcs(wfilename, filename, len);

		error(L"Could not find file \"%s\"", wfilename);

		free(wfilename);

		return editor;
	}

	TEXT_CURSOR* cursor = &editor->textCursor;
	const int bufferSize = 1000;
	char* buffer = calloc(bufferSize, sizeof(char));
	int charsRead = 0;
	do {
		charsRead = fread(buffer, sizeof(char), bufferSize, file);
		if (charsRead > 0) {
			insert_str_at_cursor(cursor, cursor->position, buffer, charsRead);
		}
	} while (charsRead == bufferSize);

	fclose(file);

	return editor;
}

void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt) {
	float t = PT_TIME_get();

	int xMargin = TEXT_EDITOR_get_margin(editor);

	TEXT_CURSOR* textCursor = &editor->textCursor;
	int cursorY = textCursor->position.y;
	int cloneLineY = cursorY + textCursor->cloneLineOffset;

	int startY = min(cursorY, cloneLineY);
	int endY = max(cursorY, cloneLineY);
	for (int y = startY; y <= endY; y++) {
		vec2i cursorPos = textCursor->position;

		TEXT_LINE thisLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(editor->textLines, y);
		int x = min(thisLine.numChars, editor->textCursor.position.x);

		PT_GUI_OBJ* cursorObj = (PT_GUI_OBJ*)textCursor->cursorFrame->subInstance;
		cursorObj->position = PT_REL_DIM_new(
			0, xMargin + get_text_width(editor->charSet, thisLine.str, x),
			0, cursorPos.y * (editor->linePadding + editor->textHeight)
		); 

		if (textCursor->insert) {
			cursorObj->size = PT_REL_DIM_new(0, editor->charWidth, 0, editor->textHeight + editor->linePadding);
		}
		else {
			cursorObj->size = PT_REL_DIM_new(0, 2, 0, editor->textHeight + editor->linePadding);
		}

		float lastTypedTime = textCursor->lastTypedTime;
		int flashIndex = floorf((t - lastTypedTime) / textCursor->flashInterval);
		if (flashIndex != textCursor->lastFlashIndex) {
			textCursor->lastFlashIndex = flashIndex;

			cursorObj->visible = flashIndex % 2 == 0;
		}
	}
}