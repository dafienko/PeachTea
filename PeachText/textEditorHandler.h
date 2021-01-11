#ifndef TEXT_EDITOR_HANDLER_H
#define TEXT_EDITOR_HANDLER_H

#include "PeachTea.h"
#include "textCursor.h"

typedef struct {
	char* str;
	int numChars;
	int numCharSpace;
} TEXT_LINE;

typedef struct {
	int textHeight, charWidth, linePadding;
	char_set* charSet;

	PT_SCROLLFRAME* scrollFrame;
	PT_RENDERFRAME* renderFrame;

	PT_COLOR insertionColor;
	float insertionFadeTime;

	PT_EXPANDABLE_ARRAY* textLines;
	TEXT_CURSOR textCursor;
} TEXT_EDITOR;

char get_last_char(TEXT_LINE line);

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe, PT_RENDERFRAME* renderFrame);
TEXT_EDITOR* TEXT_EDITOR_from_file(Instance* scrollframe, PT_RENDERFRAME* renderFrame, const char* filename);
int TEXT_EDITOR_get_margin(TEXT_EDITOR* editor);

void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt);

int* get_charsTyped();

#endif