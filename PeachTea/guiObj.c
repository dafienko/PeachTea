#include "guiObj.h"
#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "PeachTea.h"

Instance* PT_GUI_OBJ_new() {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)calloc(1, sizeof(PT_GUI_OBJ));
	obj->visible = 1;

	Instance* instance = new_instance();
	
	instance->subInstance = obj;
	instance->destroySubInstance = PT_GUI_OBJ_destroy;
	instance->instanceType = IT_GUI_OBJ;
	obj->instance = (Instance*)instance;

	return instance;
}


PT_GUI_DIMS PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_GUI_DIMS parentDims) {
	glUseProgram(frameProg);

	Instance* instance = obj->instance;

	vec2i frameSize = scale_and_offset_to_screen(obj->scale_percent, obj->scale_px, parentDims.absoluteSize);
	vec2i relFramePos = scale_and_offset_to_screen(obj->pos_percent, obj->pos_px, parentDims.absoluteSize);
	vec2i framePos = vector_add_2i(relFramePos, parentDims.absolutePos);

	if (obj->visible) {
		vec2i borderSize = (vec2i){ obj->borderWidth * 2, obj->borderWidth * 2 };
		vec2i totalFrameSize = vector_add_2i(frameSize, borderSize);
		vec2i topLeft = (vec2i){ framePos.x - obj->borderWidth, framePos.y - obj->borderWidth };
		vec2i bottomRight = vector_add_2i(topLeft, totalFrameSize);
		vec2f frameBorderComposition = (vec2f){
			(float)obj->borderWidth / (float)totalFrameSize.x,
			(float)obj->borderWidth / (float)totalFrameSize.y
		};

		vec3f borderColor = (vec3f){
			obj->border_color.x / 255.0f,
			obj->border_color.y / 255.0f,
			obj->border_color.z / 255.0f
		};

		vec3f backgroundColor = (vec3f){
			obj->background_color.x / 255.0f,
			obj->background_color.y / 255.0f,
			obj->background_color.z / 255.0f
		};

		GLuint fbcLoc, bcLoc, btLoc, cLoc, tLoc, ssLoc, mpLoc, mifLoc;

		mpLoc = glGetUniformLocation(frameProg, "mousePos");
		fbcLoc = glGetUniformLocation(frameProg, "frameBorderComposition");
		bcLoc = glGetUniformLocation(frameProg, "borderColor");
		btLoc = glGetUniformLocation(frameProg, "borderTransparency");
		cLoc = glGetUniformLocation(frameProg, "color");
		tLoc = glGetUniformLocation(frameProg, "transparency");
		ssLoc = glGetUniformLocation(frameProg, "screenSize");
		mifLoc = glGetUniformLocation(frameProg, "mouseInFrame");

		vec2i relMousePos = vector_sub_2i(mousePos, mainWindowPosition);
		int mif = relMousePos.x > topLeft.x && relMousePos.x < bottomRight.x;
		mif = mif && relMousePos.y > topLeft.y && relMousePos.y < bottomRight.y;

		glUniform2i(mpLoc, relMousePos.x, relMousePos.y);
		glUniform2i(ssLoc, screenSize.x, screenSize.y);
		glUniform2f(fbcLoc, frameBorderComposition.x, frameBorderComposition.y);
		glUniform3f(bcLoc, borderColor.x, borderColor.y, borderColor.z);
		glUniform1f(btLoc, obj->borderTransparancy);
		glUniform3f(cLoc, backgroundColor.x, backgroundColor.y, backgroundColor.z);
		glUniform1f(tLoc, obj->transparency);
		glUniform1i(mifLoc, mif);

		glBindVertexArray(*qVAO);

		glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
		int quadPositions[] = {
			topLeft.x, topLeft.y,
			topLeft.x, bottomRight.y,
			bottomRight.x, bottomRight.y,
			bottomRight.x, topLeft.y
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glDrawArrays(GL_QUADS, 0, 4);
	}

	PT_GUI_DIMS dims = { 0 };
	dims.absolutePos = framePos;
	dims.absoluteSize = frameSize;
	return dims;
}

void PT_GUI_OBJ_destroy(void* obj) {
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)obj; // obj should be a PT_GUI_OBJ
	free(guiObj);
}


