#include "guiObj.h"
#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "PeachTea.h"
#include "errorUtil.h"

#include "glUniformUtil.h"

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


PT_ABS_DIM PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_ABS_DIM parentDims) {
	glUseProgram(frameProg);

	Instance* instance = obj->instance;

	vec2i frameSize = calculate_screen_dimension(obj->size, parentDims.size);
	vec2i relFramePos = calculate_screen_dimension(obj->position, parentDims.size);
	vec2i framePos = vector_add_2i(relFramePos, parentDims.position);

	if (obj->visible) {
		vec2i borderSize = (vec2i){ obj->borderWidth * 2, obj->borderWidth * 2 };
		vec2i totalFrameSize = vector_add_2i(frameSize, borderSize);
		vec2i topLeft = (vec2i){ framePos.x - obj->borderWidth, framePos.y - obj->borderWidth };
		vec2i bottomRight = vector_add_2i(topLeft, totalFrameSize);
		vec2f frameBorderComposition = (vec2f){
			(float)obj->borderWidth / (float)totalFrameSize.x,
			(float)obj->borderWidth / (float)totalFrameSize.y
		};

		GLuint fbcLoc, bcLoc, btLoc, cLoc, tLoc, ssLoc, mpLoc, mifLoc;
		GLuint rLoc, aborcLoc, abaccLoc, abordLoc, abacdLoc;

		// general property uniforms
		mpLoc = glGetUniformLocation(frameProg, "mousePos");
		fbcLoc = glGetUniformLocation(frameProg, "frameBorderComposition");
		bcLoc = glGetUniformLocation(frameProg, "borderColor");
		btLoc = glGetUniformLocation(frameProg, "borderTransparency");
		cLoc = glGetUniformLocation(frameProg, "color");
		tLoc = glGetUniformLocation(frameProg, "backgroundTransparency");
		ssLoc = glGetUniformLocation(frameProg, "screenSize");
		mifLoc = glGetUniformLocation(frameProg, "mouseInFrame");

		vec2i relMousePos = vector_sub_2i(mousePos, mainWindowPosition);
		int mif = relMousePos.x > topLeft.x && relMousePos.x < bottomRight.x;
		mif = mif && relMousePos.y > topLeft.y && relMousePos.y < bottomRight.y;		

		uniform_vec2i(mpLoc, relMousePos);
		uniform_vec2i(ssLoc, screenSize);
		uniform_vec2f(fbcLoc, frameBorderComposition);
		uniform_PT_COLOR(bcLoc, obj->borderColor);
		glUniform1f(btLoc, obj->borderTransparancy);
		uniform_PT_COLOR(cLoc, obj->backgroundColor);
		glUniform1f(tLoc, obj->backgroundTransparency);
		glUniform1i(mifLoc, mif);

		// reactive property uniforms
		rLoc = glGetUniformLocation(frameProg, "reactive");
		aborcLoc = glGetUniformLocation(frameProg, "activeBorderColor");
		abaccLoc = glGetUniformLocation(frameProg, "activeBackgroundColor");
		abordLoc = glGetUniformLocation(frameProg, "activeBorderRange");
		abacdLoc = glGetUniformLocation(frameProg, "activeBackgroundRange");

		glUniform1i(rLoc, obj->reactive);
		uniform_PT_COLOR(aborcLoc, obj->activeBorderColor);
		uniform_PT_COLOR(abaccLoc, obj->activeBackgroundColor);
		uniform_vec2f(abordLoc, obj->activeBorderRange);
		uniform_vec2f(abacdLoc, obj->activeBackgroundRange);

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

	PT_ABS_DIM dims = { 0 };
	dims.position = framePos;
	dims.size = frameSize;
	return dims;
}

void PT_GUI_OBJ_destroy(void* obj) {
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)obj; // obj should be a PT_GUI_OBJ
	free(guiObj);
}


