#define _CRT_SECURE_NO_WARNINGS

#include "textEditorRenderer.h"
#include "textEditorHandler.h"

#include <stdlib.h>
#include <stdio.h>

void render_text_editor(TEXT_EDITOR textEditor) {
	PT_RENDERFRAME* renderFrame = textEditor.renderFrame;
	PT_canvas canvas = renderFrame->guiObj->lastCanvas;

	vec2i canvasSize = canvas_size(canvas);

	int xMargin = textEditor.charWidth * 5;
	int xPos = xMargin;
	int yPos = textEditor.textHeight;

	char* lineNumStr = calloc(20, sizeof(char));

	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		// render line number
		memset(lineNumStr, 0, 20 * sizeof(char));
		sprintf(lineNumStr, "%i", y + 1);
		int lineNumStrWidth = get_text_width(textEditor.charSet, lineNumStr);
		render_text(
			canvasSize, 
			textEditor.charSet, 
			PT_COLOR_fromRGB(35, 149, 161), 
			0, 
			lineNumStr, 
			xMargin - (lineNumStrWidth + 10), yPos
		);


		TEXT_LINE line = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textEditor.textLines, y);
		
		render_text(
			canvasSize,
			textEditor.charSet,
			PT_COLOR_new(1, 1, 1),
			0,
			line.str,
			xPos, yPos
		);

		yPos += textEditor.textHeight + textEditor.linePadding;
	}

	free(lineNumStr);
}