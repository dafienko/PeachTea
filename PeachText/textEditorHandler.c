#include "textEditorHandler.h"
#include "PeachTea.h";
#include "textCursor.h"

#include <stdlib.h>
#include <stdio.h>

TEXT_EDITOR* currentTextEditor;
PT_EXPANDABLE_ARRAY* lines;

int keyDownBound = 0;
void on_char_typed(void* args) {
	char* c = (char*)args;
	
	for (int i = currentTextEditor->textCursors->numElements - 1; i >= 0; i--) {
		TEXT_CURSOR* cursor = (TEXT_CURSOR*)PT_EXPANDABLE_ARRAY_get(currentTextEditor->textCursors, i);
		vec2i cursorPos = cursor->position;

		if (c == '\b') {
			
		}
		else {
			insert_str_at_cursor(*cursor, c, 1);
		}
	}
	
}

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe) {
	TEXT_EDITOR* editor = calloc(1, sizeof(TEXT_EDITOR));

	editor->textLines = calloc(1, sizeof(PT_EXPANDABLE_ARRAY));
	*editor->textLines = PT_EXPANDABLE_ARRAY_new(50, sizeof(TEXT_LINE));

	editor->textCursors = calloc(1, sizeof(PT_EXPANDABLE_ARRAY));
	*editor->textCursors = PT_EXPANDABLE_ARRAY_new(5, sizeof(TEXT_CURSOR));

	editor->numTextLabels = 20;
	editor->textlabels = calloc(editor->numTextLabels, sizeof(Instance*));

	int textSize = 20;
	int padding = 2;

	for (int i = 0; i < editor->numTextLabels; i++) {
		Instance* textlabelInstance = PT_TEXTLABEL_new();

		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)textlabelInstance->subInstance;
		textlabel->font = PT_FONT_CONSOLA;
		textlabel->textSize = textSize;
		textlabel->textColor = PT_COLOR_new(1, 1, 1);
		textlabel->horizontalAlignment = PT_H_ALIGNMENT_LEFT;
		textlabel->verticalAlignment = PT_V_ALIGNMENT_CENTER;

		PT_GUI_OBJ* textobj = textlabel->guiObj;
		textobj->size = PT_REL_DIM_new(1.0f, -40, 0, textSize);
		textobj->anchorPosition = (vec2f){ .5f, 0 };
		textobj->position = PT_REL_DIM_new(.5f, 0, 0, (textSize + padding) * i);

		float h = ((float)rand() / (float)RAND_MAX) * 360.0f;
		textobj->backgroundColor = PT_COLOR_fromHSV(h, 1, 1);

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
	cursorObj->size = PT_REL_DIM_new(0, 2, 0, textSize + padding);

	set_instance_parent(cursorFrame, scrollframe);

	TEXT_CURSOR mainCursor = { 0 };
	mainCursor.position = (vec2i){ 0, 0 };
	mainCursor.textArray = editor->textLines;
	mainCursor.cursorFrame = cursorFrame;

	PT_EXPANDABLE_ARRAY_add(editor->textCursors, (void*)&mainCursor);



	currentTextEditor = editor;

	if (!keyDownBound) {
		keyDownBound = 1;
		PT_BINDABLE_EVENT_bind(&eOnCharTyped, on_char_typed);
	}

	return editor;
}

void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt) {
	for (int i = 0; i < editor->numTextLabels; i++) {
		Instance* instance = *(editor->textlabels + i);
		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)instance->subInstance;

		TEXT_LINE textline = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(editor->textLines, i);
		char* str = textline.str;

		textlabel->text = str;
	}

	for (int i = 0; i < editor->textCursors->numElements; i++) {
		TEXT_CURSOR* cursor = (TEXT_CURSOR*)PT_EXPANDABLE_ARRAY_get(editor->textCursors, i);
		
		vec2i cursorPos = cursor->position;

		PT_GUI_OBJ* cursorObj = (PT_GUI_OBJ*)cursor->cursorFrame->subInstance;
		cursorObj->position = PT_REL_DIM_new(0, cursorPos.x * 10, 0, cursorPos.y * 22); // TODO: change random numbers to variables
	}
}