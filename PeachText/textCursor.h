#ifndef TEXT_CURSOR_H
#define TEXT_CURSOR_H

#include "PeachTea.h"

#include <stdlib.h>

typedef struct {
	vec2i position, selectTo;
	vec2i lastMidPos; 
	PT_COLOR color;
	PT_EXPANDABLE_ARRAY* textArray;
	int thickness;
	//Instance* cursorFrame;

	int insert;
	float lastTypedTime;
	float flashInterval;
	int lastFlashIndex;

	int cloneLineOffset, targetX;
} TEXT_CURSOR;

TEXT_CURSOR TEXT_CURSOR_new(struct TEXT_EDITOR* editor);

void TEXT_CURSOR_select_word(TEXT_CURSOR* cursor);

vec2i insert_str_in_text_array(PT_EXPANDABLE_ARRAY* textLines, vec2i pos, char* str, int len);
void remove_str_from_text_array(PT_EXPANDABLE_ARRAY* textLines, vec2i start, vec2i end);

int is_text_pos_in_range(vec2i p, vec2i start, vec2i end);
void get_cursor_selection_bounds(TEXT_CURSOR cursor, vec2i* startOut, vec2i* endOut);

void get_text_in_range(PT_EXPANDABLE_ARRAY* textArray, vec2i start, vec2i end, char** textOut, int* lengthOut);
void get_cursor_selection(TEXT_CURSOR* cursor, char** selectionOut, int* selectionLengthOut);

vec3i calculate_text_position(PT_EXPANDABLE_ARRAY* textArray, vec2i pos, vec2i dir, int targetX);
void move_cursor(TEXT_CURSOR* cursor, vec2i dir, int shiftDown, int altDown);

void update_targetX(TEXT_CURSOR* cursor);

#endif