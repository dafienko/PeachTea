#include "textCursor.h"
#include "textEditorHandler.h"

TEXT_CURSOR TEXT_CURSOR_new(TEXT_EDITOR* editor) {
	Instance* cursorFrame = PT_GUI_OBJ_new();
	PT_GUI_OBJ* cursorObj = (PT_GUI_OBJ*)cursorFrame->subInstance;
	cursorObj->size = PT_REL_DIM_new(0, 2, 0, editor->textHeight + editor->linePadding);
	cursorObj->zIndex = 6;
	cursorObj->backgroundColor = PT_COLOR_fromHSV(0, 0, .9f);

	set_instance_parent(cursorFrame, editor->scrollFrame->instance);

	TEXT_CURSOR cursor = { 0 };
	cursor.position = (vec2i){ 0, 0 };
	cursor.textArray = editor->textLines;
	cursor.cursorFrame = cursorFrame;
	cursor.flashInterval = .5f;

	return cursor;
}

int is_text_pos_in_range(vec2i p, vec2i start, vec2i end) {
	int afterRightBound = (p.x >= start.x && p.y == start.y) || p.y > start.y;
	int beforeLeftBound = (p.x <= end.x && p.y == end.y) || p.y < end.y;
	
	return afterRightBound && beforeLeftBound;
}

// splits str by '\n' and adds each string and its length to linesOut and lengthsOut
void format_insert_str(const char* str, int strLen, char*** linesOut, int** lengthsOut, int* numLinesOut) {
	// count lines
	int numLines = 1;
	for (int i = 0; i < strLen; i++) {
		char c = *(str + i);
		if (c == '\n') {
			numLines++;
		}
	}

	char** lines = calloc(numLines, sizeof(char*));
	int* lengths = calloc(numLines, sizeof(int));

	char* tempStr = calloc(strLen, sizeof(char));
	int tempLen = 0;

	int lineIndex = 0;
	for (int i = 0; i < strLen; i++) {
		char c = *(str + i);
		if (c == '\r') {
			continue;
		}

		*(tempStr + tempLen) = c;
		tempLen++;

		// clone current line str and add line to lines array
		if (c == '\n') {
			char* line = calloc(tempLen + 1, sizeof(char));

			if (tempLen > 0) {
				memcpy(line, tempStr, tempLen * sizeof(char));
			}

			*(lines + lineIndex) = line;
			*(lengths + lineIndex) = tempLen;

			tempLen = 0;
			lineIndex++;
		}

		if (i == strLen - 1) {// at the last character
			char* line = calloc(tempLen + 1, sizeof(char));

			if (tempLen > 0) {
				memcpy(line, tempStr, tempLen * sizeof(char));
			}

			*(lines + lineIndex) = line;
			*(lengths + lineIndex) = tempLen;
		}
	}

	free(tempStr);

	*numLinesOut = numLines;
	*linesOut = lines;
	*lengthsOut = lengths;
}

void remove_str_at_cursor(TEXT_CURSOR* cursor, vec2i start, vec2i end) {
	float time = PT_TIME_get();
	cursor->lastTypedTime = time;

	TEXT_LINE startLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, start.y);
	TEXT_LINE endLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, end.y);
	
	// get the strings of the lines before and after the deletion range
	int beforeLen = start.x;
	char* beforeStr = calloc(beforeLen + 1, sizeof(char));
	if (beforeLen > 0) {
		memcpy(beforeStr, startLine.str, beforeLen * sizeof(char));
	}

	int afterLen = endLine.numChars - end.x;
	char* afterStr = calloc(afterLen + 1, sizeof(char));
	if (afterLen > 0) {
		memcpy(afterStr, endLine.str + end.x, afterLen * sizeof(char));
	}

	// 'startLine' str will be replaced, everything after 'startLine' through 'endLine' get deleted
	free(startLine.str);

	startLine.numCharSpace = beforeLen + afterLen + 1; // add one for null terminator
	startLine.str = calloc(startLine.numCharSpace, sizeof(char)); 
	startLine.numChars = 0;

	if (beforeLen > 0) {
		memcpy(startLine.str + startLine.numChars, beforeStr, beforeLen * sizeof(char));
		startLine.numChars += beforeLen;
	}
	if (afterLen > 0) {
		memcpy(startLine.str + startLine.numChars, afterStr, afterLen * sizeof(char));
		startLine.numChars += afterLen;
	}
	free(beforeStr);
	free(afterStr);

	PT_EXPANDABLE_ARRAY_set(cursor->textArray, start.y, (void*)&startLine);

	// delete lines in between start and end
	for (int i = end.y; i >= start.y + 1; i--) {
		TEXT_LINE* textLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, i);
		free(textLine->str);
		PT_EXPANDABLE_ARRAY_remove(cursor->textArray, i);
	}

	// move cursor to start of deletion range
	cursor->position = start;
	cursor->selectTo = start;
	cursor->targetX = start.x;
}

void get_cursor_selection_bounds(TEXT_CURSOR cursor, vec2i* startOut, vec2i* endOut) {
	vec2i cPos = cursor.position;
	vec2i sPos = cursor.selectTo;
	if (cPos.y < sPos.y || (cPos.y == sPos.y && cPos.x < sPos.x)) { // selecTo is after cursor position
		*startOut = cPos;
		*endOut = sPos;
	}
	else { // selectTo is before cursor position
		*startOut = sPos;
		*endOut = cPos;
	}
}

void delete_cursor_selection(TEXT_CURSOR* cursor) {
	if (!vector_equal_2i(cursor->selectTo, cursor->position)) { // if there is actually something selected...
		vec2i start, end;
		get_cursor_selection_bounds(*cursor, &start, &end);
		cursor->position = start;
		remove_str_at_cursor(cursor, start, end);
	}
}

void get_cursor_selection(TEXT_CURSOR* cursor, char** selectionOut, int* selectionLengthOut) {
	vec2i start, end;
	char* selection;
	int selectionLength = 0;

	get_cursor_selection_bounds(*cursor, &start, &end);

	int numLines = (end.y - start.y) + 1; // add 1, if start/end are on the same line, there is still 1 line selected

	char** lines = calloc(numLines, sizeof(char*));
	int* lengths = calloc(numLines, sizeof(char*));

	// copy all selected lines
	for (int y = start.y; y <= end.y; y++) {
		TEXT_LINE* line = PT_EXPANDABLE_ARRAY_get(cursor->textArray, y);
		
		int xi = y == start.y ? start.x : 0;
		int xf = y == end.y ? end.x : line->numChars;

		int numChars = xf - xi;

		char* str = calloc(numChars + 1, sizeof(char)); // add 1 for null terminator
		memcpy(str, line->str + xi, numChars * sizeof(char));

		selectionLength += numChars;

		*(lengths + (y - start.y)) = numChars;
		*(lines + (y - start.y)) = str;
	}

	
	// concatenate all lines into one single big line
	int charIndex = 0;
	selectionLength += 5; // add some extra chars for null terminator
	selection = calloc(selectionLength, sizeof(char)); 
	for (int i = 0; i < numLines; i++) {
		char* line = *(lines + i);
		int length = *(lengths + i);

		memcpy(selection + charIndex, line, length * sizeof(char));
		charIndex += length;

		free(line);
	}

	*selectionOut = selection;
	*selectionLengthOut = selectionLength;

	free(lines);
	free(lengths);
}

void insert_str_at_cursor(TEXT_CURSOR* cursor, vec2i pos, char* str, int len) {
	float time = PT_TIME_get();
	cursor->lastTypedTime = time;

	delete_cursor_selection(cursor);

	TEXT_LINE* currentLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, pos.y);

	int beforeLen = pos.x;
	char* beforeStr = calloc(beforeLen + 1, sizeof(char));
	if (beforeLen > 0) {
		memcpy(beforeStr, currentLine->str, beforeLen * sizeof(char)); // copy everything before cursor
	}

	int afterLen = (currentLine->numChars - pos.x);
	char* afterStr = calloc(afterLen + 1, sizeof(char));
	if (afterLen > 0) {
		memcpy(afterStr, currentLine->str + beforeLen, afterLen * sizeof(char)); // copy everything before cursor
	}

	char** strLines;
	int* lengths;
	int numLines = 0;
	format_insert_str(str, len, &strLines, &lengths, &numLines);

	for (int i = 0; i < numLines; i++) {
		int l = *(lengths + i);
		char* line = *(strLines + i);

		TEXT_LINE insertLine = { 0 };
		insertLine.numChars = 0;
		insertLine.numCharSpace = beforeLen + l + afterLen + 1; // may only need 'l', but just to be safe
		insertLine.str = calloc(insertLine.numCharSpace, sizeof(char));

		if (i == 0) { // first line - prepend beforeStr to this text line
			if (beforeLen > 0) {
				memcpy(insertLine.str + insertLine.numChars, beforeStr, beforeLen * sizeof(char));
				insertLine.numChars += beforeLen;
			}
		}

		// add str text between beforeStr and afterStr
		if (l > 0) {
			memcpy(insertLine.str + insertLine.numChars, line, l * sizeof(char));
			insertLine.numChars += l;
		}

		if (i == numLines - 1) { // last line - append afterStr to this text line
			if (afterLen > 0) {
				memcpy(insertLine.str + insertLine.numChars, afterStr, afterLen * sizeof(char));
				insertLine.numChars += afterLen;
			}
		}

		if (i == 0) { // if this is the first line, free the existing line and replace array's index with new line
			free(currentLine->str);
			PT_EXPANDABLE_ARRAY_set(cursor->textArray, pos.y, (void*)&insertLine);
		}
		else { // if this isn't the first line, insert this text line into the array
			if (pos.y + i >= cursor->textArray->numElements) {
				PT_EXPANDABLE_ARRAY_add(cursor->textArray, (void*)&insertLine);
			}
			else {
				PT_EXPANDABLE_ARRAY_insert(cursor->textArray, pos.y + i, (void*)&insertLine);
			}
		}

		free(line);
	}

	TEXT_LINE* lastLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, pos.y + numLines - 1);
	cursor->position = (vec2i){
		lastLine->numChars - afterLen,
		pos.y + numLines - 1
	};
	cursor->selectTo = cursor->position;
	cursor->targetX = cursor->position.x;
	

	free(beforeStr);
	free(afterStr);

	free(strLines);
	free(lengths);
}

// z is new targetX;
vec3i calculate_text_position(PT_EXPANDABLE_ARRAY* textArray, vec2i pos, vec2i dir, int targetX) {
	vec3i newPos = (vec3i){ pos.x, pos.y, targetX };

	int thisY = pos.y;
	int lastY = max(thisY - 1, 0);
	int nextY = min(thisY + 1, textArray->numElements - 1);

	TEXT_LINE thisLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textArray, thisY);
	TEXT_LINE lastLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textArray, lastY);
	TEXT_LINE nextLine = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textArray, nextY);

	if (dir.x == 1) { // move right
		newPos.x++;

		int numChars = thisLine.numChars;
		if (get_last_char(thisLine) == '\n') {
			numChars--;
		}

		if (newPos.x > numChars) {
			if (nextY > thisY) {
				newPos.x = 0;
				newPos.y = nextY;
			}
			else {
				newPos.x = numChars;
			}
		}

		newPos.z = newPos.x;
	}
	else if (dir.x == -1) { // move left
		newPos.x--;

		if (newPos.x < 0) {
			if (lastY < thisY) {
				newPos.x = lastLine.numChars - 1; // move cursor to before the last line's '\n' character
				newPos.y = lastY;
			}
			else {
				newPos.x = 0;
			}
		}

		newPos.z = newPos.x;
	}
	else if (dir.y == 1) { // move up
		if (thisY > lastY) { // if pos isn't on the first line (the cursor has a line to move up to)
			newPos.y = lastY;

			int lastLineNumChars = lastLine.numChars;
			if (get_last_char(lastLine) == '\n') {
				lastLineNumChars--;
			}
			newPos.x = min(targetX, lastLineNumChars);
		}
		else { // pos is on the first line
			newPos.x = 0; // move to the beginning of first line
			newPos.z = newPos.x;
		}
	}
	else if (dir.y == -1) { // move down
		if (thisY < nextY) { // if pos isn't on the last line (the cursor has a line to move down to)
			newPos.y = nextY;

			int nextLineNumChars = nextLine.numChars;
			if (get_last_char(nextLine) == '\n') {
				nextLineNumChars--;
			}
			newPos.x = min(targetX, nextLineNumChars);
		}
		else { // pos is on the last line
			newPos.x = nextLine.numChars; // move to end of last line line
			newPos.z = newPos.x;
		}
	}

	return newPos;
}

/*
u/d/l/r -> move cursor position; bring cursor selection position to cursor position

alt + u/d -> drag selected lines up/down

shift + u/d/l/r -> move cursor selection position

alt + shift + u/d -> move cursor clone line offset
*/
void move_cursor(TEXT_CURSOR* cursor, vec2i dir, int shiftDown, int altDown) {
	float time = PT_TIME_get();
	cursor->lastTypedTime = time;
	
	vec3i newPosData = calculate_text_position(cursor->textArray, cursor->position, dir, cursor->targetX);

	vec2i newPos = (vec2i){ newPosData.x, newPosData.y };

	// there is at least one line clone and at least alt or shift is being held down
	if (abs(cursor->cloneLineOffset) > 0 && (altDown || shiftDown)) {
		int dy = newPos.y - cursor->position.y;
		cursor->cloneLineOffset -= dy;
	}
	else if (altDown && !shiftDown) { // drag selection with cursor

	}
	else if (!(shiftDown || altDown)) {
		cursor->selectTo = newPos;
		cursor->cloneLineOffset = 0;
	}

	cursor->position = newPos;
	cursor->targetX = newPosData.z;
}
