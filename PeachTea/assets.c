#define _CRT_SECURE_NO_WARNINGS

#include "fileUtil.h"
#include "errorUtil.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_WIDTH 500

#define FREAD_BUFFER_SIZE 10000

int get_num_lines(FILE* file) {
	int numLines = 1;

	char buffer[FREAD_BUFFER_SIZE];
	int numRead = 0;
	do {
		numRead = fread(buffer, sizeof(char), FREAD_BUFFER_SIZE, file);

		for (int i = 0; i < numRead; i++) {
			if (buffer[i] == '\n') {
				numLines++;
			}
		}
	} while (numRead == FREAD_BUFFER_SIZE);

	fseek(file, 0, SEEK_SET);
	return numLines;
}

void get_file_lines(const char* filename, int* numLines, char*** linesOut, int** lengthsOut) {
	FILE* file;
	fopen_s(&file, filename, "rb");
	if (file == NULL) {
		int e = errno;
		int wSize = strlen(filename) + 5;
		wchar_t* wFilename = calloc(wSize, sizeof(wchar_t));
		mbstowcs(wFilename, filename, wSize);
		error(L"error opening file \"%s\": %s", wFilename, _wcserror(e));
		free(wFilename);
		exit(e);
		return;
	}

	*numLines = get_num_lines(file);
	char** lines = calloc(*numLines, sizeof(char*));
	int* lengths = calloc(*numLines, sizeof(int));

	char c;
	char* currentLine = calloc(MAX_LINE_WIDTH, sizeof(char));
	int lineCharIndex = 0;
	int lineNum = 0;

	char buffer[FREAD_BUFFER_SIZE];
	int numRead = 0;
	do {
		memset(buffer, 0, FREAD_BUFFER_SIZE * sizeof(char));
		numRead = fread(buffer, sizeof(char), FREAD_BUFFER_SIZE, file);

		for (int i = 0; i < numRead; i++) {
			char c = buffer[i];

			*(currentLine + lineCharIndex) = c;
			lineCharIndex++;

			if (c == '\n' || (numRead != FREAD_BUFFER_SIZE && i == numRead - 1)) { // if the character is the end of a line, add this line to the lines array and reset for the next line
				if (lineCharIndex > 0) {
					char* line = calloc(lineCharIndex + 1, sizeof(char)); // add 1 for null terminator
					memcpy(line, currentLine, lineCharIndex);
					*(lines + lineNum) = line;
					*(lengths + lineNum) = lineCharIndex;

					lineNum++;
				}

				lineCharIndex = 0;
			}
		}
	} while (numRead == FREAD_BUFFER_SIZE);

	*numLines = lineNum;
	*linesOut = lines;
	*lengthsOut = lengths;

	free(currentLine);
	fclose(file);
}

void freeLines(const int numLines, char*** lines) {
	for (int i = 0; i < numLines; i++) {
		free(*(*lines + i));
	}

	free(*lines);
	*lines = NULL;
}