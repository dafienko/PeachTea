#ifndef TEXT_CURSOR_H
#define TEXT_CURSOR_H

#include "PeachTea.h"
#include "textEditorHandler.h"

#include <stdlib.h>

typedef struct {
	vec2i position;
	PT_EXPANDABLE_ARRAY* textArray;
	Instance* cursorFrame;
} TEXT_CURSOR;

void insert_str_at_cursor(TEXT_CURSOR* cursor, char* str, int len);
void remove_str_at_cursor(TEXT_CURSOR* cursor, vec2i start, vec2i end);

#endif