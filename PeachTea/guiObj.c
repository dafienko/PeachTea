#include "guiObj.h"
#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "PeachTea.h"
#include "errorUtil.h"
#include "PeachTeaShaders.h"

#include "glUniformUtil.h"

#include <stdio.h>

int get_instance_zindex(Instance* inst) {
	int z = 0;

	switch (inst->instanceType) {
	case IT_GUI_OBJ:
		;
		PT_GUI_OBJ* obj = (PT_GUI_OBJ*)inst->subInstance;
		z = obj->zIndex;
		break;
	case IT_IMAGELABEL:
		;
		PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)inst->subInstance;
		z = imagelabel->guiObj->zIndex;
		break;
	case IT_TEXTLABEL:
		;
		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)inst->subInstance;
		z = textlabel->guiObj->zIndex;
		break;
	}

	return z;
}

void compare_instances(void* v1, void* v2) {
	Instance* i1 = *((Instance**)v1);
	Instance* i2 = *((Instance**)v2);

	int z1 = get_instance_zindex(i1);
	int z2 = get_instance_zindex(i2);

	if (z1 > z2) {
		return 1;
	}
	else if (z1 < z2) {
		return -1;
	}
	else {
		return 0;
	}
}

void onDescendantsChanged(void* args) {
	Instance* instance = (Instance*)args;
	
	Instance** children = instance->children;

	quicksort(children, sizeof(Instance*), instance->numChildren, compare_instances);
}

Instance* PT_GUI_OBJ_new() {
	PT_GUI_OBJ* obj = calloc(1, sizeof(PT_GUI_OBJ));

	obj->visible = 1;

	Instance* instance = new_instance();
	
	instance->subInstance = obj;
	instance->destroySubInstance = PT_GUI_OBJ_destroy;
	instance->instanceType = IT_GUI_OBJ;
	obj->instance = (Instance*)instance;

	obj->sizeConstraint = PT_SIZE_CONSTRAINT_none();

	PT_BINDABLE_EVENT_bind(&instance->childAdded, onDescendantsChanged);
	PT_BINDABLE_EVENT_bind(&instance->childRemoved, onDescendantsChanged);

	return instance;
}

PT_GUI_OBJ* PT_GUI_OBJ_clone(PT_GUI_OBJ* source, Instance* instanceClone) {
	PT_GUI_OBJ* clone = calloc(1, sizeof(PT_GUI_OBJ));

	memcpy(clone, source, sizeof(PT_GUI_OBJ));

	clone->instance = instanceClone;
	clone->sizeConstraint = PT_SIZE_CONSTRAINT_clone(source->sizeConstraint);

	PT_BINDABLE_EVENT_bind(&instanceClone->childAdded, onDescendantsChanged);
	PT_BINDABLE_EVENT_bind(&instanceClone->childRemoved, onDescendantsChanged);

	return clone;
}

PT_ABS_DIM PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_ABS_DIM parentDims) {
	glUseProgram(PTS_guiObj);

	Instance* instance = obj->instance;

	vec2i frameSize = obj->sizeConstraint->calculateSize(obj, parentDims);
	vec2i relPos = calculate_screen_dimension(obj->position, parentDims.size);
	vec2i anchorPos = vector_add_2i(relPos, parentDims.position);
	vec2i anchorOffset = (vec2i){
		obj->anchorPosition.x * frameSize.x,
		obj->anchorPosition.y * frameSize.y
	};
	vec2i framePos = vector_sub_2i(anchorPos, anchorOffset);

	if (obj->visible) {
		vec2i borderSize = (vec2i){ obj->borderWidth * 2, obj->borderWidth * 2 };
		vec2i totalFrameSize = vector_add_2i(frameSize, borderSize);
		vec2i topLeft = (vec2i){ framePos.x - obj->borderWidth, framePos.y - obj->borderWidth };
		vec2i bottomRight = vector_add_2i(topLeft, totalFrameSize);
		vec2f frameBorderComposition = (vec2f){
			(float)obj->borderWidth / (float)totalFrameSize.x,
			(float)obj->borderWidth / (float)totalFrameSize.y
		};

		GLuint fbcLoc, bcLoc, btLoc, cLoc, tLoc, ssLoc, mpLoc, mifLoc, dLoc;
		GLuint rLoc, aborcLoc, abaccLoc, abordLoc, abacdLoc;

		// general property uniforms
		mpLoc = glGetUniformLocation(PTS_guiObj, "mousePos");
		fbcLoc = glGetUniformLocation(PTS_guiObj, "frameBorderComposition");
		bcLoc = glGetUniformLocation(PTS_guiObj, "borderColor");
		btLoc = glGetUniformLocation(PTS_guiObj, "borderTransparency");
		cLoc = glGetUniformLocation(PTS_guiObj, "backgroundColor");
		tLoc = glGetUniformLocation(PTS_guiObj, "backgroundTransparency");
		ssLoc = glGetUniformLocation(PTS_guiObj, "screenSize");
		mifLoc = glGetUniformLocation(PTS_guiObj, "mouseInFrame");
		dLoc = glGetUniformLocation(PTS_guiObj, "depth");

		int mif = mousePos.x > topLeft.x && mousePos.x < bottomRight.x; // "Mouse In Frame"
		mif = mif && mousePos.y > topLeft.y && mousePos.y < bottomRight.y;

		float depth;
		if (parentDims.sortingType == ZST_SIBLING) {
			depth = 0;
		}
		else {
			depth = obj->zIndex / 128.0f;
		}

		uniform_vec2i(mpLoc, mousePos);
		uniform_vec2i(ssLoc, screenSize);
		uniform_vec2f(fbcLoc, frameBorderComposition);
		uniform_PT_COLOR(bcLoc, obj->borderColor);
		glUniform1f(btLoc, obj->borderTransparancy);
		uniform_PT_COLOR(cLoc, obj->backgroundColor);
		glUniform1f(tLoc, obj->backgroundTransparency);
		glUniform1i(mifLoc, mif);
		glUniform1f(dLoc, depth);

		// reactive property uniforms
		rLoc = glGetUniformLocation(PTS_guiObj, "reactive");
		aborcLoc = glGetUniformLocation(PTS_guiObj, "activeBorderColor");
		abaccLoc = glGetUniformLocation(PTS_guiObj, "activeBackgroundColor");
		abordLoc = glGetUniformLocation(PTS_guiObj, "activeBorderRange");
		abacdLoc = glGetUniformLocation(PTS_guiObj, "activeBackgroundRange");

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(DEFAULT_QUAD_CORNERS), DEFAULT_QUAD_CORNERS, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glDrawArrays(GL_QUADS, 0, 4);
	}

	PT_ABS_DIM dims = { 0 };
	dims.position = framePos;
	dims.size = frameSize;
	dims.sortingType = parentDims.sortingType;

	obj->lastAbsoluteDim = dims;

	return dims;
}

void PT_GUI_OBJ_destroy(void* obj) {
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)obj; // obj should be a PT_GUI_OBJ
	free(guiObj);
}


