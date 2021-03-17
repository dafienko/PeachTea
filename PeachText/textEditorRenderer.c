#define _CRT_SECURE_NO_WARNINGS

#include "textEditorRenderer.h"
#include "textEditorHandler.h"
#include "PeachTea.h"
#include "ui.h"

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
	int lineThickness = editorTextHeight + editorLinePadding;

	textEditor.sideRenderFrame->guiObj->size = PT_REL_DIM_new(0, xMargin - 5, 1, 0);


	if (!textEditor.sideRenderFrame->renderTexture.tex) {
		return; // can't render line numbers if side render frame's texture is null
	}

	vec2i frameSize = canvas_size(textEditor.sideRenderFrame->guiObj->lastCanvas);

	// render line # side bar
	PT_FRAMETEXTURE_bind(textEditor.sideRenderFrame->renderTexture);
	PT_FRAMETEXTURE_clear(textEditor.sideRenderFrame->renderTexture, colorTheme.backgroundColor);

	///*
	glUseProgram(PTS_rect);
	uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), colorTheme.sidebarColor);
	glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), sideFrameTransparency);
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
	//*/

	glUseProgram(PTS_text);
	char* lineNumStr = calloc(30, sizeof(char));
	///*
	// render line numbers
	for (int i = 0; i < lineNumbers->numElements; i++) {
		lineNumber lnum = *(lineNumber*)PT_EXPANDABLE_ARRAY_get(lineNumbers, i);

		memset(lineNumStr, 0, 30 * sizeof(char));
		sprintf(lineNumStr, "%i", lnum.l + 1);

		// render line number
		int numDigits = (int)strlen(lineNumStr);
		int lineNumStrWidth = get_text_rect(editorCharSet, lineNumStr, numDigits, 0).x;
		PT_COLOR fringeColor = PT_TWEEN_PT_COLOR(colorTheme.sidebarColor, colorTheme.backgroundColor, sideFrameTransparency, PT_LINEAR, PT_IN);
		uniform_PT_COLOR(
			glGetUniformLocation(PTS_text, "fringeColor"),
			fringeColor
		);
		
		render_text(
			frameSize,
			editorCharSet,
			colorTheme.accentColor, //PT_COLOR_fromRGB(43, 145, 175),
			0,
			lineNumStr,
			numDigits,
			xMargin - (lineNumStrWidth + 10), lnum.lineStartBaselineY - canvasOffset.y,
			0, lineThickness,
			NULL
		);
	}
	free(lineNumStr);
	//*/
}

void render_selection_rects(PT_EXPANDABLE_ARRAY* arr, vec2i canvasOffset) {
	glUseProgram(PTS_rect);

	uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), screenSize);
	uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), colorTheme.accentColor);
	glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), .4f);

	default_quad_corners();

	for (int i = 0; i < arr->numElements / 4; i++) {
		float alpha = (float)i / ((float)arr->numElements / 4.0f);

		vec2i topRight = *(vec2i*)PT_EXPANDABLE_ARRAY_get(arr, i * 4 + 0);
		vec2i topLeft = *(vec2i*)PT_EXPANDABLE_ARRAY_get(arr, i * 4 + 1);
		vec2i bottomLeft = *(vec2i*)PT_EXPANDABLE_ARRAY_get(arr, i * 4 + 2);
		vec2i bottomRight = *(vec2i*)PT_EXPANDABLE_ARRAY_get(arr, i * 4 + 3);

		set_quad_positions(
			vector_sub_2i(topLeft, canvasOffset),
			vector_sub_2i(bottomRight, canvasOffset)
		);

		glDrawArrays(GL_QUADS, 0, 4);
	}
}

void add_quad_to_arr(PT_EXPANDABLE_ARRAY* arr, vec2i topLeft, vec2i bottomRight) {
	vec2i topRight = (vec2i){ bottomRight.x, topLeft.y };
	vec2i bottomLeft = (vec2i){ topLeft.x, bottomRight.y };

	// add points to arr in counter-clockwise winding pattern
	PT_EXPANDABLE_ARRAY_add(arr, (void*)&topRight); 
	PT_EXPANDABLE_ARRAY_add(arr, (void*)&topLeft); 
	PT_EXPANDABLE_ARRAY_add(arr, (void*)&bottomLeft); 
	PT_EXPANDABLE_ARRAY_add(arr, (void*)&bottomRight); 
}

void render_text_editor(TEXT_EDITOR* textEditor) {
	float time = PT_TIME_get();

	PT_SCROLLFRAME* scrollFrame = textEditor->scrollFrame;
	vec2i canvasOffset = scrollFrame->canvasPosition;
	PT_canvas scrollCanvas = scrollFrame->guiObj->lastCanvas;
	vec2i scrollFrameSize = canvas_size(scrollCanvas);
	vec2i scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, scrollFrameSize);

	PT_RENDERFRAME* renderFrame = textEditor->renderFrame;
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

	int xMargin = scrollCanvas.left + TEXT_EDITOR_get_margin(textEditor);
	int lineThickness = editorTextHeight + editorLinePadding;

	int baselineX = xMargin - canvasOffset.x;

	// render text
	glUseProgram(PTS_text);
	glUniform1i(glGetUniformLocation(PTS_text, "useClipBounds"), 0);

	PT_EXPANDABLE_ARRAY lineNumbers = PT_EXPANDABLE_ARRAY_new(1, sizeof(lineNumber));

	int wrapX = TEXT_EDITOR_get_wrapX(textEditor);
	int relWrapX = 0;
	if (wrapX) {
		relWrapX = wrapX - baselineX;
	}
	int yPos = editorTextHeight + editorLinePadding / 2;;
	int maxX = xMargin;
	int maxY = textEditor->textLines->numElements * (editorLinePadding + editorTextHeight);
	int cursorPositioned = 0;

	vec2i sStart, sEnd;
	get_cursor_selection_bounds(textEditor->textCursor, &sStart, &sEnd);
	PT_EXPANDABLE_ARRAY selectionRects = PT_EXPANDABLE_ARRAY_new(50, sizeof(vec2i));

	for (int y = 0; y < textEditor->textLines->numElements; y++) {
		TEXT_LINE* pLine = (TEXT_LINE*)PT_EXPANDABLE_ARRAY_get(textEditor->textLines, y);
		TEXT_LINE line = *pLine;
		vec2i rect = get_text_rect(editorCharSet, line.str, line.numChars, relWrapX);

		// render cursor position and highlight selected line
		TEXT_CURSOR cursor = textEditor->textCursor;
		if (y == cursor.position.y) {
			vec2i offset = get_text_offset(editorCharSet, line.str, cursor.position.x, relWrapX);
			vec2i cursorPos = (vec2i){
				baselineX + offset.x,
				(yPos - lineThickness) + offset.y * lineThickness + editorLinePadding / 2 - canvasOffset.y
			};
			vec2i cursorSize = (vec2i){ cursor.thickness, lineThickness };
			textEditor->textCursor.lastMidPos = vector_add_2i(cursorPos, vector_div_2i(cursorSize, 2));
			cursorPositioned = 1;

			glUseProgram(PTS_rect);
			uniform_vec2i(glGetUniformLocation(PTS_rect, "screenSize"), screenSize);
			glUniform1f(glGetUniformLocation(PTS_rect, "transparency"), 0);

			default_quad_corners();
			uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), colorTheme.selectedLineColor);

			// render selected line background highlight
			set_quad_positions(
				(vec2i) {
				0, yPos - canvasOffset.y - lineThickness + editorLinePadding / 2
				},
				(vec2i) {
					screenSize.x, (yPos + (rect.y) * lineThickness) - canvasOffset.y + editorLinePadding / 2
				}
			);
			glDrawArrays(GL_QUADS, 0, 4);

			// render cursor
			float timeSinceEdit = time - cursor.lastTypedTime;
			float flashR = fmod(timeSinceEdit, cursor.flashInterval * 2);
			if (flashR < cursor.flashInterval) {
				uniform_PT_COLOR(glGetUniformLocation(PTS_rect, "color"), colorTheme.cursorColor);
				set_quad_positions(cursorPos, vector_add_2i(cursorPos, cursorSize));
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}


		// if this line is even visible
		if (yPos + (1 + rect.y) * lineThickness > occlusionTopLeftBound.y && yPos <= occlusionBottomRightBound.y) {

			lineNumber lnum = { 0 };
			lnum.l = y;
			lnum.lineStartBaselineY = yPos;
			PT_EXPANDABLE_ARRAY_add(&lineNumbers, &lnum);

			// calculate cursor selection rects
			if (y >= sStart.y && y <= sEnd.y && !vector_equal_2i(sStart, sEnd)) { // a part of this line is selected
				int lineTop = yPos + -lineThickness + editorLinePadding / 2;
				vec2i sTopLeft = { 0, lineTop };
				vec2i sBottomRight = { 0 };

				int lineStartIndex = 0;
				if (y == sStart.y) {
					lineStartIndex = sStart.x;
				}
				vec2i startOffset = get_text_offset(editorCharSet, line.str, lineStartIndex, relWrapX);
				if (y == sStart.y) {
					sTopLeft.x = startOffset.x + baselineX;
					sTopLeft.y = lineTop + startOffset.y * lineThickness;
				}
				else {
					sTopLeft.x = baselineX;
				}

				int lineEndIndex = line.numChars;
				int lastCharWasNewline = 0;
				if (y == sEnd.y) {
					lineEndIndex = sEnd.x;
				}
				else if (get_last_char(line) == '\n') {
					lineEndIndex--;
					lastCharWasNewline = 1;
				}


				vec2i endOffset = get_text_offset(editorCharSet, line.str, lineEndIndex, relWrapX);
				int endLineNum = endOffset.y;
				sBottomRight.x = baselineX + endOffset.x;
				sBottomRight.y = lineTop + (endOffset.y + 1) * lineThickness;

				if (lastCharWasNewline) {
					sBottomRight.x += 10;
				}

				if (startOffset.y == endOffset.y) { // if this line doesn't have any text-wrapping going on
					add_quad_to_arr(&selectionRects, sTopLeft, sBottomRight);
				}
				else { // this line is being wrapped
					vec2i firstLineBR = (vec2i){ baselineX + rect.x, sTopLeft.y + lineThickness };
					add_quad_to_arr(&selectionRects, sTopLeft, firstLineBR);

					vec2i midTL = (vec2i){ baselineX, sTopLeft.y + lineThickness };
					vec2i midBR = (vec2i){ baselineX + rect.x, sBottomRight.y - lineThickness };
					if (midTL.y < midBR.y) {
						add_quad_to_arr(&selectionRects, midTL, midBR);
					}

					vec2i bottomTL = (vec2i){ baselineX, sBottomRight.y - lineThickness };
					add_quad_to_arr(&selectionRects, bottomTL, sBottomRight);
				}
			}

			PT_EXPANDABLE_ARRAY* lineFlags = &pLine->flags;
			TEXT_METADATA_FLAG lastFlag = { 0 }; 
			for (int i = lineFlags->numElements - 1; i >= 0; i--) {
				TEXT_METADATA_FLAG* flag = PT_EXPANDABLE_ARRAY_get(lineFlags, i);
				float timeDiff = time - *(float*)flag->misc;
				float alpha = min(1.0f, timeDiff / textEditor->editFadeTime);

				if (i > 0) {
					TEXT_METADATA_FLAG* nextFlag = PT_EXPANDABLE_ARRAY_get(lineFlags, i - 1);
					float nextTimeDiff = time - *(float*)nextFlag->misc;
					float nextAlpha = min(1.0f, nextTimeDiff / textEditor->editFadeTime);


					if (nextAlpha == alpha) { // if the flag at i-1 is the same color as this flag, then destroy it (it's redundant)
						free(flag->misc);
						PT_EXPANDABLE_ARRAY_remove(lineFlags, i);
					}
					else {
						flag->color = PT_COLOR_lerp(colorTheme.editColor, colorTheme.textColor, alpha);
					}
				}
				else {
					flag->color = PT_COLOR_lerp(colorTheme.editColor, colorTheme.textColor, alpha);
				}
			}

			// render line text
			if (line.numChars > 0) {
				///*
				if (y == cursor.position.y) {
					uniform_PT_COLOR(glGetUniformLocation(PTS_text, "fringeColor"), colorTheme.selectedLineColor);
				}
				else {
					uniform_PT_COLOR(glGetUniformLocation(PTS_text, "fringeColor"), colorTheme.backgroundColor);
				}

				render_text(
					canvasSize,
					editorCharSet,
					colorTheme.textColor,
					0,
					line.str,
					line.numChars,
					baselineX, yPos - canvasOffset.y,
					wrapX, lineThickness,
					&line.flags
				);
				//*/
			}
		}

		if (yPos > occlusionBottomRightBound.y && cursorPositioned) {
			break;
		}

		maxX = max(maxX, rect.x);
		yPos += (rect.y + 1) * lineThickness;
	}

	maxY = max(yPos, maxY);

	render_selection_rects(&selectionRects, canvasOffset);

	render_line_numbers(*textEditor, canvasSize, occlusionTopLeftBound, occlusionBottomRightBound, &lineNumbers);

	PT_EXPANDABLE_ARRAY_destroy(&selectionRects);
	PT_EXPANDABLE_ARRAY_destroy(&lineNumbers);

	// adjust canvas size to maximum line lengths
	if (wrapX) {
		scrollFrame->canvasSize = PT_REL_DIM_new(1.0f, 0, .9f, maxY);
		scrollFrame->canvasPosition = (vec2i){ 
			0,
			scrollFrame->canvasPosition.y
		};
		scrollFrame->targetCanvasPosition = scrollFrame->canvasPosition;
	}
	else {
		maxX = max(maxX, occlusionBottomRightBound.x - occlusionTopLeftBound.x);
		maxY = max(maxY, occlusionBottomRightBound.y - occlusionTopLeftBound.y);
		scrollFrame->canvasSize = PT_REL_DIM_new(0.1f, maxX, .9f, maxY);
		scrollCanvasSize = calculate_screen_dimension(scrollFrame->canvasSize, scrollFrameSize);

		scrollFrame->canvasPosition = (vec2i){ // make sure canvas position doesn't over-extend canvas size after canvas is resized
			max(0, min(scrollFrame->canvasPosition.x, scrollCanvasSize.x - scrollFrameSize.x)),
			scrollFrame->canvasPosition.y
		};
	}
}