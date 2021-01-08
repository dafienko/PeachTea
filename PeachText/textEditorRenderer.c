#define _CRT_SECURE_NO_WARNINGS

#include "textEditorRenderer.h"
#include "textEditorHandler.h"
#include "PeachTea.h"

#include <stdlib.h>
#include <stdio.h>

void render_text_editor(TEXT_EDITOR textEditor) {
	PT_RENDERFRAME* renderFrame = textEditor.renderFrame;
	PT_canvas canvas = renderFrame->guiObj->lastCanvas;

	vec2i canvasSize = canvas_size(canvas);


	int xMargin = textEditor.charWidth * 5;
	int xPos = xMargin;
	int yPos = textEditor.textHeight;

	int maxX = xMargin;
	int maxY = yPos;

	char* lineNumStr = calloc(20, sizeof(char));


	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		// render line number
		memset(lineNumStr, 0, 20 * sizeof(char));
		sprintf(lineNumStr, "%i", y + 1);
		int lineNumStrWidth = get_text_width(textEditor.charSet, lineNumStr, strlen(lineNumStr));
		render_text(
			canvasSize,
			textEditor.charSet,
			PT_COLOR_fromRGB(35, 149, 161),
			0,
			lineNumStr,
			strlen(lineNumStr),
			xMargin - (lineNumStrWidth + 10), yPos
		);


		TEXT_LINE line = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textEditor.textLines, y);
		
		if (textEditor.textLines->numElements > 5) {
			int x = 0;
		}

		if (line.numChars > 0) {
			render_text(
				canvasSize,
				textEditor.charSet,
				PT_COLOR_new(1, 1, 1),
				0,
				line.str,
				line.numChars,
				xPos, yPos
			);

			xPos += get_text_width(textEditor.charSet, line.str, line.numChars);
		}

		yPos += textEditor.textHeight + textEditor.linePadding;

		maxX = max(xPos, maxX);
		maxY = max(yPos, maxY);
	}


	textEditor.scrollFrame->canvasSize = PT_REL_DIM_new(0.1, maxX, .9, maxY);
	textEditor.renderFrame->guiObj->size = PT_REL_DIM_new(0, maxX + 50, 0, maxY + 50);

	free(lineNumStr);
}