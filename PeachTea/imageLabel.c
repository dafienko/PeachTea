#include "imageLabel.h"
#include "PeachTeaShaders.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "glUniformUtil.h"
#include "mouse.h"
#include <stdlib.h>

Instance* PT_IMAGELABEL_new() {
	PT_IMAGELABEL* imgLabel = calloc(1, sizeof(PT_IMAGELABEL));
	imgLabel->visible = 1;
	imgLabel->image.texId = -1;

	Instance* instance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;

	imgLabel->guiObj = obj;
	imgLabel->instance = instance;
	instance->subInstance = (void*)imgLabel;
	instance->instanceType = IT_IMAGELABEL;

	return instance;
}

PT_IMAGELABEL* PT_IMAGELABEL_clone(PT_IMAGELABEL* source, Instance* instanceClone) {
	PT_IMAGELABEL* clone = calloc(1, sizeof(PT_IMAGELABEL));

	memcpy(clone, source, sizeof(PT_IMAGELABEL));
	clone->instance = instanceClone;
	clone->guiObj = PT_GUI_OBJ_clone(source->guiObj, instanceClone);

	return clone;
}

PT_ABS_DIM PT_IMAGELABEL_render(PT_IMAGELABEL* img, PT_ABS_DIM parentDims) {
	img->guiObj->visible = img->visible;
	PT_ABS_DIM childDims = PT_GUI_OBJ_render(img->guiObj, parentDims);

	if (img->visible) {
		glUseProgram(PTS_img);

		vec2i topLeft = childDims.position;
		vec2i bottomRight = vector_add_2i(topLeft, childDims.size);

		int ssLoc = glGetUniformLocation(PTS_img, "screenSize");
		int tLoc = glGetUniformLocation(PTS_img, "imageTransparency");
		int itLoc = glGetUniformLocation(PTS_img, "imageTint");
		int itaLoc = glGetUniformLocation(PTS_img, "imageTintAlpha");
		int mpLoc = glGetUniformLocation(PTS_img, "mousePos");
		int mifLoc = glGetUniformLocation(PTS_img, "mouseInFrame");
		int rLoc = glGetUniformLocation(PTS_img, "reactive");
		int abcLoc = glGetUniformLocation(PTS_img, "activeBackgroundColor");
		int abrLoc = glGetUniformLocation(PTS_img, "activeBackgroundRange");

		int mif = mousePos.x > topLeft.x && mousePos.x < bottomRight.x; // "Mouse In Frame"
		mif = mif && mousePos.y > topLeft.y && mousePos.y < bottomRight.y;

		glUniform2i(ssLoc, screenSize.x, screenSize.y);
		glUniform1f(tLoc, img->imageTransparency);
		uniform_PT_COLOR(itLoc, img->imageTint);
		glUniform1f(itaLoc, img->imageTintAlpha);
		uniform_vec2i(mpLoc, mousePos);
		glUniform1i(mifLoc, mif);
		glUniform1i(rLoc, img->reactive);
		uniform_PT_COLOR(abcLoc, img->activeBackgroundColor);
		uniform_vec2f(abrLoc, img->activeBackgroundRange);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, img->image.texId);

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

		PT_IMAGE image = img->image;

		float quadCorners[] = {
			image.topLeft.x / (float)image.totalImageSize.x, image.topLeft.y / (float)image.totalImageSize.y, // top left
			image.topLeft.x / (float)image.totalImageSize.x, image.bottomRight.y / (float)image.totalImageSize.y, // bottom left
			image.bottomRight.x / (float)image.totalImageSize.x, image.bottomRight.y / (float)image.totalImageSize.y, // bottom right
			image.bottomRight.x / (float)image.totalImageSize.x, image.topLeft.y / (float)image.totalImageSize.y, // top right
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners), quadCorners, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glDrawArrays(GL_QUADS, 0, 4);
	}

	return childDims;
}

void PT_IMAGELABEL_destroy(void* obj) {
	PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)obj;

	PT_GUI_OBJ_destroy((void*)imagelabel->guiObj);

	free(imagelabel);
}