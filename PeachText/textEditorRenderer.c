#define _CRT_SECURE_NO_WARNINGS

#include "textEditorRenderer.h"
#include "textEditorHandler.h"
#include "PeachTea.h"

#include <stdlib.h>
#include <stdio.h>

void render_text_editor(TEXT_EDITOR textEditor) {
	PT_SCROLLFRAME* scrollFrame = textEditor.scrollFrame;
	vec2i canvasOffset = scrollFrame->canvasPosition;
	PT_canvas scrollCanvas = scrollFrame->guiObj->lastCanvas;
	vec2i scrollFrameSize = canvas_size(scrollCanvas);
	vec2i scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, scrollFrameSize);

	PT_RENDERFRAME* renderFrame = textEditor.renderFrame;
	PT_canvas canvas = renderFrame->guiObj->lastCanvas;

	vec2i canvasSize = canvas_size(canvas);
	
	vec2i occlusionTopLeftBound = (vec2i){
		scrollFrame->canvasPosition.x,
		scrollFrame->canvasPosition.y
	};
	vec2i occlusionBottomRightBound = (vec2i){
		scrollFrame->canvasPosition.x + scrollFrameSize.x,
		scrollFrame->canvasPosition.y + screenSize.y
	};

	int xMargin = scrollCanvas.left + TEXT_EDITOR_get_margin(&textEditor);
	int lineThickness = textEditor.textHeight + textEditor.linePadding;

	// render selection bounding box
	TEXT_CURSOR cursor = textEditor.textCursor;
	if (!vector_equal_2i(cursor.position, cursor.selectTo)) { // if there is some selected text;
		glUseProgram(PTS_rect);

		uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), screenSize);
		uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), accentColor);
		glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), .2f);

		if (cursor.cloneLineOffset == 0) {
			vec2i start, end;
			get_cursor_selection_bounds(cursor, &start, &end);

			for (int y = start.y; y <= end.y; y++) {
				int yPos = y * lineThickness;
				if (yPos + lineThickness > occlusionTopLeftBound.y) {
					TEXT_LINE line = *(TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textEditor.textLines, y);
					int xi = 0;
					int xf = line.numChars;

					if (y == start.y) {
						xi = start.x;
					}

					if (y == end.y) {
						xf = end.x;
					}

					int xiPos = xMargin + get_text_width(textEditor.charSet, line.str, xi);
					int xfPos = xMargin + get_text_width(textEditor.charSet, line.str, xf);
					if (xf == 1 && y < end.y) {
						xfPos += textEditor.charWidth / 2; // signify selection of newline
					}

					// if this selection box's top edge is out of the scrollframe, stop rendering selection box (it isn't visible)
					if (yPos > occlusionBottomRightBound.y) {
						break;
					}

					default_quad_corners();
					set_quad_positions(
						(vec2i) {
							xiPos - canvasOffset.x, yPos - canvasOffset.y
						},
						(vec2i) {
							xfPos - canvasOffset.x, yPos + lineThickness - canvasOffset.y
						}
					);

					glDrawArrays(GL_QUADS, 0, 4);
				}
			}
		}
	}


	// render text
	int xPos = xMargin;
	int yPos = textEditor.textHeight;
	int maxX = xMargin;
	int maxY = textEditor.textLines->numElements * (textEditor.linePadding + textEditor.textHeight);
	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		if (yPos > occlusionTopLeftBound.y) {
			if (yPos - lineThickness > occlusionBottomRightBound.y) { // if the y pos is out of the occlusion bound, stop rendering (it isn't visible)
				break;
			}
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
					xPos - canvasOffset.x, yPos - canvasOffset.y
				);

				xPos += get_text_width(textEditor.charSet, line.str, line.numChars);
			}

			maxX = max(xPos, maxX);
		}

		yPos += lineThickness;
	}

	// render line # side bar
	glUseProgram(PTS_rect);
	uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), scrollFrame->guiObj->backgroundColor);
	glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), 0.1);
	uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), screenSize);
	default_quad_corners();
	set_quad_positions(
		(vec2i) {
			0, 0
		},
		(vec2i) {
			xMargin - 5, screenSize.y
		}
	);
	glDrawArrays(GL_QUADS, 0, 4);


	// render line numbers
	yPos = textEditor.textHeight;
	char* lineNumStr = calloc(20, sizeof(char));
	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		if (yPos > occlusionTopLeftBound.y) {
			if (yPos - lineThickness > occlusionBottomRightBound.y) { // if the y pos is out of the occlusion bound, stop rendering (it isn't visible)
				break;
			}

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
				xMargin - (lineNumStrWidth + 10), yPos - canvasOffset.y
			);
		}

		yPos += lineThickness;
	}

	// adjust canvas size to maximum line lengths
	scrollFrame->canvasSize = PT_REL_DIM_new(0.1, maxX, .9, maxY);
	scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, scrollFrameSize);

	scrollFrame->canvasPosition = (vec2i) { // make sure canvas position doesn't over-extend canvas size after canvas is resized
		max(0, min(scrollFrame->canvasPosition.x, scrollCanvasSize.x - scrollFrameSize.x)),
		scrollFrame->canvasPosition.y
	};

	free(lineNumStr);
}