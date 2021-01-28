#ifndef TEXT_CURSOR_H
#define TEXT_CURSOR_H

#include "PeachTea.h"

#include <stdlib.h>

typedef struct {
	vec2i position, selectTo;
	PT_EXPANDABLE_ARRAY* textArray;
	Instance* cursorFrame;

	int insert;
	float lastTypedTime;
	float flashInterval;
	int lastFlashIndex;

	int cloneLineOffset, targetX;
} TEXT_CURSOR;

TEXT_CURSOR TEXT_CURSOR_new(struct TEXT_EDITOR* editor);

void TEXT_CURSOR_select_word(TEXT_CURSOR* cursor);

void insert_str_at_cursor(TEXT_CURSOR* cursor, char* str, int len);
void remove_str_at_cursor(TEXT_CURSOR* cursor, vec2i start, vec2i end);

int is_text_pos_in_range(vec2i p, vec2i start, vec2i end);
void get_cursor_selection_bounds(TEXT_CURSOR cursor, vec2i* startOut, vec2i* endOut);

void delete_cursor_selection(TEXT_CURSOR* cursor);
void get_cursor_selection(TEXT_CURSOR* cursor, char** selectionOut, int* selectionLengthOut);

vec3i calculate_text_position(PT_EXPANDABLE_ARRAY* textArray, vec2i pos, vec2i dir, int targetX);
void move_cursor(TEXT_CURSOR* cursor, vec2i dir, int shiftDown, int altDown);

#endif