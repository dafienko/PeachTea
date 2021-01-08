#ifndef TEXT_CURSOR_H
#define TEXT_CURSOR_H

#include "PeachTea.h"

#include <stdlib.h>

typedef struct {
	vec2i position, selectTo;
	PT_EXPANDABLE_ARRAY* textArray;
	Instance* cursorFrame;

	int cloneLineOffset, targetX;
} TEXT_CURSOR;

void insert_str_at_cursor(TEXT_CURSOR* cursor, vec2i pos, char* str, int len);
void remove_str_at_cursor(TEXT_CURSOR* cursor, vec2i start, vec2i end);

vec3i calculate_text_position(PT_EXPANDABLE_ARRAY* textArray, vec2i pos, vec2i dir, int targetX);
void move_cursor(TEXT_CURSOR* cursor, vec2i dir, int shiftDown, int altDown);

#endif