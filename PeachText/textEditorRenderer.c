#define _CRT_SECURE_NO_WARNINGS

#include "textEditorRenderer.h"
#include "textEditorHandler.h"
#include "PeachTea.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int l;
	int lineStartBaselineY;
} lineNumber;


void render_line_numbers(TEXT_EDITOR textEditor, vec2i canvasSize, vec2i occlusionTopLeftBound, vec2i occlusionBottomRightBound, PT_EXPANDABLE_ARRAY* lineNumbers) {
	PT_SCROLLFRAME* scrollFrame = textEditor.scrollFrame;
	vec2i canvasOffset = scrollFrame->canvasPosition;
	PT_canvas scrollCanvas = scrollFrame->guiObj->lastCanvas;
	
	int xMargin = scrollCanvas.left + TEXT_EDITOR_get_margin(&textEditor);
	int lineThickness = textEditor.textHeight + textEditor.linePadding;

	textEditor.sideRenderFrame->guiObj->size = PT_REL_DIM_new(0, xMargin - 5, 1, 0);

	if (!textEditor.sideRenderFrame->renderTexture.tex) {
		return; // can't render line numbers if side render frame's texture is null
	}

	vec2i frameSize = canvas_size(textEditor.sideRenderFrame->guiObj->lastCanvas);

	// render line # side bar
	PT_FRAMETEXTURE_bind(textEditor.sideRenderFrame->renderTexture);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(PTS_rect);
	uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), scrollFrame->guiObj->backgroundColor);
	//uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), PT_COLOR_fromRGB(255, 0, 0));
	glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), 0.4f);
	uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), frameSize);
	
	default_quad_corners();
	
	set_quad_positions(
			(vec2i) {
			0, 0
		},
			(vec2i) {
			xMargin - 5, frameSize.y
		}
	);
	glDrawArrays(GL_QUADS, 0, 4);
	

	char* lineNumStr = calloc(30, sizeof(char));
	//*
	// render line numbers
	for (int i = 0; i < lineNumbers->numElements; i++) {
		lineNumber lnum = *(lineNumber*)PT_EXPANDABLE_ARRAY_get(lineNumbers, i);

		memset(lineNumStr, 0, 30 * sizeof(char));
		sprintf(lineNumStr, "%i", lnum.l + 1);

		// render line number
		int lineNumStrWidth = get_text_rect(textEditor.charSet, lineNumStr, strlen(lineNumStr), 0).x;
		render_text(
			frameSize,
			textEditor.charSet,
			accentColor, //PT_COLOR_fromRGB(43, 145, 175),
			0,
			lineNumStr,
			strlen(lineNumStr),
			xMargin - (lineNumStrWidth + 10), lnum.lineStartBaselineY - canvasOffset.y,
			0, lineThickness
		);
	}
	free(lineNumStr);
	//*/
}

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

	int baselineX = xMargin - canvasOffset.x;

	// render selection bounding boxg
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
				int yPos = y * lineThickness; // +textEditor.linePadding / 2;
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

					int xiPos = xMargin + get_text_rect(textEditor.charSet, line.str, xi, 0).x;
					int xfPos = xMargin + get_text_rect(textEditor.charSet, line.str, xf, 0).x;
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
	PT_EXPANDABLE_ARRAY lineNumbers = PT_EXPANDABLE_ARRAY_new(1, sizeof(lineNumber));
	
	int wrapX = TEXT_EDITOR_get_wrapX(&textEditor) ;
	int xPos = xMargin;
	int yPos = textEditor.textHeight + textEditor.linePadding / 2;;
	int maxX = xMargin;
	int maxY = textEditor.textLines->numElements * (textEditor.linePadding + textEditor.textHeight);
	for (int y = 0; y < textEditor.textLines->numElements; y++) {
		xPos = xMargin;

		if (yPos > occlusionTopLeftBound.y) {
			if (yPos - lineThickness > occlusionBottomRightBound.y) { // if the y pos is out of the occlusion bound, stop rendering (it isn't visible)
				break;
			}
			lineNumber lnum = { 0 };
			lnum.l = y;
			lnum.lineStartBaselineY = yPos;
			PT_EXPANDABLE_ARRAY_add(&lineNumbers, &lnum);

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
					baselineX, yPos - canvasOffset.y,
					screenSize.x - scrollFrame->scrollBarThickness, lineThickness
				);

				vec2i rect = get_text_rect(textEditor.charSet, line.str, line.numChars, wrapX);
				xPos += rect.x;
				yPos += rect.y * lineThickness;
			}

			maxX = max(xPos, maxX);
		}

		yPos += lineThickness;
	}

	render_line_numbers(textEditor, canvasSize, occlusionTopLeftBound, occlusionBottomRightBound, &lineNumbers);

	PT_EXPANDABLE_ARRAY_destroy(&lineNumbers);

	// adjust canvas size to maximum line lengths
	maxX = max(maxX, occlusionBottomRightBound.x - occlusionTopLeftBound.x);
	maxY = max(maxY, occlusionBottomRightBound.y - occlusionTopLeftBound.y);
	scrollFrame->canvasSize = PT_REL_DIM_new(0.1, maxX, .9, maxY);
	scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, scrollFrameSize);

	scrollFrame->canvasPosition = (vec2i) { // make sure canvas position doesn't over-extend canvas size after canvas is resized
		max(0, min(scrollFrame->canvasPosition.x, scrollCanvasSize.x - scrollFrameSize.x)),
		scrollFrame->canvasPosition.y
	};
}