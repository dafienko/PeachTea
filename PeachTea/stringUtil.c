#include <string.h>
#include <stdlib.h>
#include "stringUtil.h"

typedef unsigned char byte;

void* clone_memory(void* source, int numBytes) {
	byte* clone = calloc(numBytes, 1);

	memcpy(clone, source, numBytes);

	return (void*)clone;
}

char* create_heap_str(const char* source) {
	return (char*)clone_memory(source, (strlen(source) + 1) * sizeof(char));
}

wchar_t* create_heap_wstr(const wchar_t* source) {
	return (wchar_t*)clone_memory(source, (strlen(source) + 1) * sizeof(wchar_t));
}

void getStrLines(const char* str, char*** linesOut, int* numLinesOut) {
	int len = strlen(str);

	int linesBufferSize = 1;
	int numLines = 0;
	char** lines = calloc(linesBufferSize, sizeof(char*));

	int bufferSize = 10;
	int numUsedBufferChars = 0;
	char* lineBuffer = calloc(bufferSize, sizeof(char));

	int lastLineIndex = -1;
	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (numUsedBufferChars + 2 > bufferSize) {
			bufferSize *= 2;
			lineBuffer = realloc(lineBuffer, bufferSize * sizeof(char));
		}

		*(lineBuffer + numUsedBufferChars) = c;
		numUsedBufferChars++;

		if (c == '\n' || i == len - 1) {
			if (numLines + 1 > linesBufferSize) {
				linesBufferSize *= 2;
				lines = realloc(lines, linesBufferSize * sizeof(char*));
			}

			lineBuffer = realloc(lineBuffer, (numUsedBufferChars + 1) * sizeof(char));
			*(lineBuffer + numUsedBufferChars) = 0;
			*(lines + numLines) = lineBuffer;
			numLines++;

			if (i != len - 1) {
				bufferSize = 10;
				numUsedBufferChars = 0;
				lineBuffer = calloc(bufferSize, sizeof(char));
			}
		}
	}

	lines = realloc(lines, numLines * sizeof(char*));

	*linesOut = lines;
	*numLinesOut = numLines;
}

void free_lines(char** lines, int numlines) {
	for (int i = 0; i < numlines; i++) {
		char* line = *(lines + i);
		free(line);
	}

	free(lines);
}