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

	// render selection bounding box
	TEXT_CURSOR cursor = textEditor.textCursor;
	if (!vector_equal_2i(cursor.position, cursor.selectTo)) { // if there is some selected text;
		glUseProgram(PTS_rect);

		uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), canvasSize);
		uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), accentColor);
		glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), .1f);

		if (cursor.cloneLineOffset == 0) {
			vec2i start, end;
			get_cursor_selection_bounds(cursor, &start, &end);

			for (int y = start.y; y <= end.y; y++) {
				TEXT_LINE line = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textEditor.textLines, y);
				int xi = 0;
				int xf = line.numChars;

				if (y == start.y) {
					xi = start.x;
				}

				if (y == end.y) {
					xf = end.x;
				}

				int lineThickness = textEditor.textHeight + textEditor.linePadding;
				int yPos = y * lineThickness;
				int xiPos = textEditor.charWidth * 5 + get_text_width(textEditor.charSet, line.str, xi);
				int xfPos = textEditor.charWidth * 5 + get_text_width(textEditor.charSet, line.str, xf);
				if (xf == 1 && y < end.y) {
					xfPos += textEditor.charWidth / 2; // signify selection of newline
				}

				default_quad_corners();
				set_quad_positions(
					(vec2i) {xiPos, yPos}, 
					(vec2i) {xfPos, yPos + lineThickness}
				);

				glDrawArrays(GL_QUADS, 0, 4);
			}
		}
	}
	

	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		// render line number
		memset(lineNumStr, 0, 20 * sizeof(char));
		sprintf(lineNumStr, "%i", y + 1);
		int lineNumStrWidth = get_text_width(textEditor.charSet, lineNumStr, strlen(lineNumStr));
		render_text(
			canvasSize,
			textEditor.charSet,
			accentColor, //PT_COLOR_fromRGB(43, 145, 175),
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
				PT_COLOR_fromHSV(0, 0, .8),
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