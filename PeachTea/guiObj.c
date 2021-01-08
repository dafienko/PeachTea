#include "guiObj.h"
#include "ScreenUI.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "PeachTea.h"
#include "errorUtil.h"
#include "PeachTeaShaders.h"
#include "ScreenUI.h"

#include "glUniformUtil.h"

#include <stdlib.h>
#include <stdio.h>

int get_instance_zindex(Instance* instance) {
	PT_GUI_OBJ* guiObj = NULL;

	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		guiObj = instance->subInstance;
		break;
	case IT_IMAGELABEL:
		;
		PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)instance->subInstance;
		guiObj = imageLabel->guiObj;

		break;
	case IT_SCROLLFRAME:
		;
		PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)instance->subInstance;
		guiObj = scrollFrame->guiObj;

		break;
	case IT_TEXTLABEL:
		;
		PT_TEXTLABEL* textLabel = (PT_TEXTLABEL*)instance->subInstance;
		guiObj = textLabel->guiObj;

		break;
	case IT_RENDERFRAME:
		;
		PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)instance->subInstance;
		guiObj = renderFrame->guiObj;
	}

	if (guiObj) {
		return guiObj->zIndex;
	}
	else {
		return -1;
	}
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
	clone->sizeConstraint = PT_SIZE_CONSTRAINT_clone(source->sizeConstraint);

	clone->instance = instanceClone;

	if (instanceClone) {
		PT_BINDABLE_EVENT_bind(&instanceClone->childAdded, onDescendantsChanged);
		PT_BINDABLE_EVENT_bind(&instanceClone->childRemoved, onDescendantsChanged);
	}

	return clone;
}

PT_canvas PT_GUI_OBJ_update_size(PT_GUI_OBJ* obj, PT_canvas parentCanvas) {
	PT_ABS_DIM parentDims = { 0 };
	parentDims.position = canvas_pos(parentCanvas);
	parentDims.size = canvas_size(parentCanvas);

	vec2i frameSize = obj->sizeConstraint->calculateSize(obj, parentDims);
	vec2i relPos = calculate_screen_dimension(obj->position, parentDims.size);
	PT_canvas childCanvas = calculate_child_canvas(parentCanvas, relPos, frameSize, obj->anchorPosition, obj->clipDescendants);
	vec2i framePos = canvas_pos(childCanvas);

	PT_ABS_DIM dims = { 0 };
	dims.position = framePos;
	dims.size = frameSize;

	obj->lastCanvas = childCanvas;
	obj->lastAbsoluteDim = dims;

	return childCanvas;
}

void PT_GUI_OBJ_render(PT_GUI_OBJ* obj, PT_SCREEN_UI* ui) {
	vec2i frameSize = canvas_size(obj->lastCanvas);
	vec2i framePos = canvas_pos(obj->lastCanvas);

	if (obj->visible) {
		if (obj->blurred) {
			PT_FRAMETEXTURE_blur(ui->frameTexture.tex, ui->effectTexture1, (vec2f) { 0, 1 }, obj->blurRadius, 0);
			PT_FRAMETEXTURE_blur(ui->effectTexture1.tex, ui->effectTexture2, (vec2f) { 1, 0 }, obj->blurRadius, 1);
			PT_FRAMETEXTURE_bind(ui->frameTexture);
		}

		glUseProgram(PTS_guiObj);

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
		GLuint bLoc, brLoc, baLoc;

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

		float depth = 0.1f;

		// clipping bounds
		GLuint ucbLoc, clXLoc, clYLoc;
		ucbLoc = glGetUniformLocation(PTS_guiObj, "useClipBounds");
		clXLoc = glGetUniformLocation(PTS_guiObj, "clipX");
		clYLoc = glGetUniformLocation(PTS_guiObj, "clipY");
		glUniform1i(ucbLoc, obj->lastCanvas.clipDescendants);
		glUniform2i(clXLoc, obj->lastCanvas.cleft, obj->lastCanvas.cright);
		glUniform2i(clYLoc, obj->lastCanvas.ctop, obj->lastCanvas.cbottom);

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

		// blur property uniforms
		bLoc = glGetUniformLocation(PTS_guiObj, "blurred");
		glUniform1i(bLoc, obj->blurred);

		if (obj->blurred) {
			brLoc = glGetUniformLocation(PTS_guiObj, "blurRadius");
			baLoc = glGetUniformLocation(PTS_guiObj, "blurAlpha");

			glUniform1i(brLoc, obj->blurRadius);
			glUniform1f(baLoc, obj->blurAlpha);

			glUseProgram(PTS_guiObj);


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ui->effectTexture2.tex);
		}

		set_quad_positions(topLeft, bottomRight);
		default_quad_corners();

		glDrawArrays(GL_QUADS, 0, 4);
	}
}

void PT_GUI_OBJ_destroy(void* obj) {
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)obj; // obj should be a PT_GUI_OBJ
	free(guiObj);
}


