#define _CRT_SECURE_NO_WARNINGS
#include "textEditorHandler.h"
#include <stdlib.h>
#include <stdio.h>

EDITOR_STATE_CONFIG load_editor_state_config() {
	EDITOR_STATE_CONFIG config = { 0 };
	config.wordWrap = 1;
	config.openFiles = NULL;
	config.numOpenFiles = 0;

	FILE* file = fopen("config", "r");
	if (file) {
		fclose(file);

		int numLines = 0;
		int* lengths = NULL;
		char** lines = NULL;
		get_file_lines("config", &numLines, &lines, &lengths);

		PT_EXPANDABLE_ARRAY openFileNames = PT_EXPANDABLE_ARRAY_new(4, sizeof(char*));

		for (int i = 0; i < numLines; i++) {
			char* line = *(lines + i);
			int len = *(lengths + i);

			if (len > 0) {
				char firstChar = *(line + 0);
				char* lineBuffer = calloc(len, sizeof(char));
				int bufferIndex = 0;

				switch (firstChar) {
				case 'f': // this line defines an open file path
					;
					int foundQuote = 0;
					for (int j = 0; j < len; j++) {
						char c = *(line + j);
						if (c == '\"') {
							foundQuote++;
							if (foundQuote > 1) {
								break;
							}
							continue; // don't record the opening quote in the file path
						}

						if (foundQuote) {
							*(lineBuffer + bufferIndex) = c;
							bufferIndex++;
						}
					}

					char* filepath = calloc(bufferIndex + 1, sizeof(char));
					memcpy(filepath, lineBuffer, bufferIndex * sizeof(char));
					PT_EXPANDABLE_ARRAY_add(&openFileNames, &filepath);

					break;
				case 't': // this line defines the editor's text size
				case 'w': // this line defines if the text editor should have word-wrap enabled
					;
					for (int j = 0; j < len; j++) {
						char c = *(line + j);
						if (c >= 48 && c <= 57) {
							*(lineBuffer + bufferIndex) = c;
							bufferIndex++;
						}
					}

					int n = atoi(lineBuffer);

					if (firstChar == 'w') {
						config.wordWrap = n;
					}
					else if (firstChar == 't') {
						config.textSize = n;
					}
					break;
				}

				free(lineBuffer);
			}
		}


		config.numOpenFiles = openFileNames.numElements;
		config.openFiles = (char**)openFileNames.data;
	}

	return config;
}

void realize_editor_state_config(Instance* backgroundInstance, PT_RENDERFRAME* renderFrame, PT_RENDERFRAME* sideRenderFrame, PT_SCROLLFRAME* listContainer, EDITOR_STATE_CONFIG config) {
	for (int i = 0; i < config.numOpenFiles; i++) {
		char* fullpath = *(config.openFiles + i);
		TEXT_EDITOR_from_file(backgroundInstance, renderFrame, sideRenderFrame, listContainer, fullpath);
	}

	wrapText = config.wordWrap;
	editorTextHeight = config.textSize;
}

EDITOR_STATE_CONFIG get_editor_state_config() {
	EDITOR_STATE_CONFIG config = { 0 };

	config.wordWrap = wrapText;
	config.textSize = editorTextHeight;

	PT_EXPANDABLE_ARRAY editors = get_open_editors();

	config.openFiles = calloc(editors.numElements, sizeof(char*));
	int fileIndex = 0;
	for (int i = 0; i < editors.numElements; i++) {
		TEXT_EDITOR* editor = *(TEXT_EDITOR**)PT_EXPANDABLE_ARRAY_get(&editors, i);

		if (editor->path && editor->filename) {
			int fullLen = strlen(editor->path) + strlen(editor->filename);
			if (editor->extension) {
				fullLen += strlen(editor->extension);
			}

			char* fullpath = calloc(fullLen + 10, sizeof(char));

			if (editor->extension) {
				sprintf(fullpath, "%s%s.%s", editor->path, editor->filename, editor->extension);
			}
			else {
				sprintf(fullpath, "%s%s", editor->path, editor->filename);
			}

			*(config.openFiles + fileIndex) = fullpath;
			fileIndex++;
		}

	}
	config.numOpenFiles = fileIndex;

	return config;
}

void free_editor_state_config(EDITOR_STATE_CONFIG* config) {
	for (int i = 0; i < config->numOpenFiles; i++) {
		char* fullpath = *(config->openFiles + i);
		free(fullpath);
	}

	free(config->openFiles);
	config->openFiles = NULL;
	config->numOpenFiles = 0;
}

void save_state_config(EDITOR_STATE_CONFIG config) {
	FILE* file = fopen("config", "w");

	const int bufferSize = 2000;
	char* buffer = calloc(bufferSize, sizeof(char));

	// save word-wrap data
	sprintf(buffer, "w %i\n", config.wordWrap);
	fwrite(buffer, sizeof(char), strlen(buffer), file);
	memset(buffer, 0, bufferSize * sizeof(char));
	
	// save textsize data
	sprintf(buffer, "t %i\n", config.textSize);
	fwrite(buffer, sizeof(char), strlen(buffer), file);
	memset(buffer, 0, bufferSize * sizeof(char));

	for (int i = 0; i < config.numOpenFiles; i++) {
		char* fullpath = *(config.openFiles + i);
		sprintf(buffer, "f \"%s\"\n", fullpath);
		fwrite(buffer, sizeof(char), strlen(buffer), file);
		memset(buffer, 0, bufferSize * sizeof(char));
	}

	free(buffer);

	fclose(file);
}

