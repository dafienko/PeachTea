#define _CRT_SECURE_NO_WARNINGS

#include "imageLoader.h"
#include <stdio.h>
#include <stdlib.h>
#include "errorUtil.h"

HBITMAP load_bitmap(const char* filename) {
	int pathLength = strlen(filename) + 200;
	char* fullPath = calloc(pathLength, sizeof(char));
	sprintf(fullPath, "assets\\images\\filename");
	FILE* file = fopen(fullPath, "rb");

	if (file == NULL) {
		wchar_t* wfilename = calloc(pathLength, sizeof(wchar_t));
		mbstowcs(wfilename, filename, pathLength - 1);
		fatal_error(L"could not find file \"%s\"", wfilename);
	}

	fclose(file);

	HBITMAP hbmp = LoadImageA(
		NULL,
		(LPCSTR)fullPath,
		IMAGE_BITMAP,
		0, 0,
		LR_LOADFROMFILE
	);

	free(fullPath);

	return hbmp;
}