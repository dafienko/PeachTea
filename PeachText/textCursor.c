#include "textCursor.h"

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
		else {
			*(tempStr + tempLen) = c;
			tempLen++;
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

void insert_str_at_cursor(TEXT_CURSOR cursor, char* str, int len) {
	TEXT_LINE* currentLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(cursor.textArray, cursor.position.y);

	int beforeLen = cursor.position.x;
	char* beforeStr = calloc(beforeLen + 1, sizeof(char));
	if (beforeLen > 0) {
		memcpy(beforeStr, currentLine, beforeLen * sizeof(char)); // copy everything before cursor
	}

	int afterLen = (currentLine->numChars - cursor.position.x);
	char* afterStr = calloc(afterLen + 1, sizeof(char));
	if (afterLen > 0) {
		memcpy(afterStr, currentLine + beforeLen, afterLen * sizeof(char)); // copy everything before cursor
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
		insertLine.numCharSpace = beforeLen + l + afterLen; // may only need l, but just to be safe
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
			PT_EXPANDABLE_ARRAY_set(cursor.textArray, cursor.position.y, (void*)&insertLine);
		}
		else { // if this isn't the first line, insert this text line into the array
			PT_EXPANDABLE_ARRAY_insert(cursor.textArray, cursor.position.y + i, (void*)&insertLine);
		}

		free(line);
	}

	free(beforeStr);
	free(afterStr);

	free(strLines);
	free(lengths);
}