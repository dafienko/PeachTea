#include <stdlib.h>
#include "textLabel.h"
#include "guiObj.h"
#include "ScreenUI.h"
#include "glText.h"
#include "PeachTeaShaders.h"

#include "fontHandler.h"
#include "stringUtil.h"

Instance* PT_TEXTLABEL_new() {
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)calloc(1, sizeof(PT_TEXTLABEL));
	textlabel->visible = 1;

	Instance* inst = PT_GUI_OBJ_new();
	PT_GUI_OBJ* obj1 = (PT_GUI_OBJ*)inst->subInstance;

	textlabel->instance = inst;
	textlabel->guiObj = (PT_GUI_OBJ*)inst->subInstance;
	inst->subInstance = (void*)textlabel;
	inst->instanceType = IT_TEXTLABEL;

	return inst;
}

PT_TEXTLABEL* PT_TEXTLABEL_clone(PT_TEXTLABEL* source, Instance* instanceClone) {
	PT_TEXTLABEL* clone = calloc(1, sizeof(PT_TEXTLABEL));
	memcpy(clone, source, sizeof(PT_TEXTLABEL));

	clone->text = create_heap_str(source->text);
	clone->instance = instanceClone;

	return clone;
}

void PT_TEXTLABEL_destroy(void* obj) {
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)obj;

	PT_GUI_OBJ_destroy((void*)textlabel->guiObj);

	if (textlabel->text != NULL) {
		free(textlabel->text);
	}

	free(textlabel);
}

PT_canvas PT_TEXTLABEL_update_size(PT_TEXTLABEL* textlabel, PT_canvas parentCanvas) {
	return PT_GUI_OBJ_update_size(textlabel->guiObj, parentCanvas);
}

PT_canvas PT_TEXTLABEL_render(PT_TEXTLABEL* textlabel, PT_SCREEN_UI* ui) {
	textlabel->guiObj->visible = textlabel->visible;

	PT_GUI_OBJ_render(textlabel->guiObj, ui);
	
	PT_canvas childCanvas = textlabel->guiObj->lastCanvas;
	vec2i childPos = canvas_pos(childCanvas);
	vec2i childSize = canvas_size(childCanvas);

	if (textlabel->visible) {
		char_set* cs = get_char_set(textlabel->font, textlabel->textSize);

		char** lines;
		int numLines;

		getStrLines(textlabel->text, &lines, &numLines);

		int linePadding = 3;
		int totalTextHeight = numLines * (textlabel->textSize + linePadding) - linePadding;
		int baselineY = childPos.y;
		switch (textlabel->verticalAlignment) {
		case PT_V_ALIGNMENT_CENTER:
			;
			int remainingHeight = childSize.y - totalTextHeight;
			baselineY = childPos.y + remainingHeight / 2;
			break;
		case PT_V_ALIGNMENT_BOTTOM:
			baselineY = childPos.y + childSize.y + -totalTextHeight;
			break;
		}

		for (int i = 0; i < numLines; i++) {
			char* line = *(lines + i);
			int len = strlen(line);

			int textWidth = get_text_width(cs, line);

			int baselineX = childPos.x;
			switch (textlabel->horizontalAlignment) {
			case PT_H_ALIGNMENT_CENTER:
				;
				int remainingWidth = childSize.x - textWidth;
				baselineX = childPos.x + (remainingWidth / 2);
				break;
			case PT_H_ALIGNMENT_RIGHT:
				baselineX = childPos.x + childSize.x + -textWidth;
				break;
			}

			glUseProgram(PTS_text);

			// clipping bounds
			GLuint ucbLoc, clXLoc, clYLoc;
			ucbLoc = glGetUniformLocation(PTS_text, "useClipBounds");
			clXLoc = glGetUniformLocation(PTS_text, "clipX");
			clYLoc = glGetUniformLocation(PTS_text, "clipY");
			glUniform1i(ucbLoc, textlabel->guiObj->lastCanvas.clipDescendants);
			glUniform2i(clXLoc, textlabel->guiObj->lastCanvas.cleft, textlabel->guiObj->lastCanvas.cright);
			glUniform2i(clYLoc, textlabel->guiObj->lastCanvas.ctop, textlabel->guiObj->lastCanvas.cbottom);

			render_text(
				cs, 
				textlabel->textColor, 
				textlabel->textTransparency,
				line, 
				baselineX, baselineY + (i + 1) * (textlabel->textSize + linePadding) - linePadding
			);
		}

		free_lines(lines, numLines);
	}

	return childCanvas;
}