#ifndef TEXT_EDITOR_HANDLER_H
#define TEXT_EDITOR_HANDLER_H

#include "PeachTea.h"
#include "textCursor.h"

typedef struct {
	char* str;
	int numChars, numCharSpace;
} TEXT_LINE;

typedef struct {
	int textHeight, charWidth, linePadding;
	char_set* charSet;

	PT_RENDERFRAME* renderFrame;

	PT_EXPANDABLE_ARRAY* textLines;
	TEXT_CURSOR textCursor;

	Instance** textlabels;
	int numTextLabels;
} TEXT_EDITOR;

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe, PT_RENDERFRAME* renderFrame);
void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt);

#endif