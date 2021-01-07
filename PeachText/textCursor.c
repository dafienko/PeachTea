#include "textCursor.h"
#include "textEditorHandler.h"

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
	for (int i = start.y + 1; i <= end.y; i++) {
		TEXT_LINE* textLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, i);
		free(textLine->str);
		PT_EXPANDABLE_ARRAY_remove(cursor->textArray, i);
	}

	// move cursor to start of deletion range
	cursor->position = start;
}

void insert_str_at_cursor(TEXT_CURSOR* cursor, vec2i pos, char* str, int len) {
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
			memcpy(insertLine.str + insertLine.numChars, line, len * sizeof(char));
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
			PT_EXPANDABLE_ARRAY_insert(cursor->textArray, pos.y + i, (void*)&insertLine);
		}

		free(line);
	}

	TEXT_LINE* lastLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor->textArray, pos.y + numLines - 1);
	cursor->position = (vec2i){
		lastLine->numChars - afterLen,
		pos.y + numLines - 1
	};

	free(beforeStr);
	free(afterStr);

	free(strLines);
	free(lengths);
}