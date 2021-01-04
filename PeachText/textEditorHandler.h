#ifndef TEXT_EDITOR_HANDLER_H
#define TEXT_EDITOR_HANDLER_H

#include "PeachTea.h"

typedef struct {
	char* str;
	int numChars, numCharSpace;
} TEXT_LINE;

typedef struct {
	PT_EXPANDABLE_ARRAY* textLines;
	PT_EXPANDABLE_ARRAY* textCursors;

	Instance** textlabels;
	int numTextLabels;
} TEXT_EDITOR;

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe);
void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt);

#endif