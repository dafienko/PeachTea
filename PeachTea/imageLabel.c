#include "imageLabel.h"
#include "PeachTeaShaders.h"
#include "glExtensions.h"
#include "screenSize.h"
#include "glUniformUtil.h"
#include "mouse.h"
#include <stdlib.h>
#include <stdio.h>

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
	
	instance->destroySubInstance = PT_IMAGELABEL_destroy;

	imgLabel->imageScale = 1.0f;

	return instance;
}

PT_IMAGELABEL* PT_IMAGELABEL_clone(PT_IMAGELABEL* source, Instance* instanceClone) {
	PT_IMAGELABEL* clone = calloc(1, sizeof(PT_IMAGELABEL));

	memcpy(clone, source, sizeof(PT_IMAGELABEL));
	clone->instance = instanceClone;
	clone->guiObj = PT_GUI_OBJ_clone(source->guiObj, instanceClone);

	return clone;
}

PT_canvas PT_IMAGELABEL_update_size(PT_IMAGELABEL* imgLabel, PT_canvas parentCanvas) {
	return PT_GUI_OBJ_update_size(imgLabel->guiObj, parentCanvas);
}

void PT_IMAGELABEL_render(PT_IMAGELABEL* img, PT_SCREEN_UI* ui) {
	img->guiObj->visible = img->visible;
	PT_canvas childCanvas = img->guiObj->lastCanvas;

	PT_GUI_OBJ_render(img->guiObj, ui);

	if (img->visible) {
		glUseProgram(PTS_img);

		// clipping bounds
		GLuint ucbLoc, clXLoc, clYLoc;
		ucbLoc = glGetUniformLocation(PTS_img, "useClipBounds");
		clXLoc = glGetUniformLocation(PTS_img, "clipX");
		clYLoc = glGetUniformLocation(PTS_img, "clipY");
		glUniform1i(ucbLoc, img->guiObj->lastCanvas.clipDescendants);
		glUniform2i(clXLoc, img->guiObj->lastCanvas.cleft, img->guiObj->lastCanvas.cright);
		glUniform2i(clYLoc, img->guiObj->lastCanvas.ctop, img->guiObj->lastCanvas.cbottom);

		vec2i canvasSize = canvas_size(childCanvas);
		vec2i topLeft = canvas_pos(childCanvas);
		vec2i bottomRight = vector_add_2i(topLeft, canvasSize);



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

		//printf("%.2f, %.2f, %.2f     -  %.2f\n", img->imageTint.r, img->imageTint.g, img->imageTint.b, img->imageTransparency);
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


		PT_IMAGE image = img->image;

		vec2f topLeftCorner = (vec2f){ image.topLeft.x / (float)image.totalImageSize.x, image.topLeft.y / (float)image.totalImageSize.y };
		vec2f bottomRightCorner = (vec2f){ image.bottomRight.x / (float)image.totalImageSize.x, image.bottomRight.y / (float)image.totalImageSize.y };


		vec2i canvasCenter = vector_add_2i(topLeft, vector_div_2i(canvasSize, 2));
		canvasSize = (vec2i){ canvasSize.x * img->imageScale, canvasSize.y * img->imageScale };

		topLeft = vector_sub_2i(canvasCenter, vector_div_2i(canvasSize, 2));
		bottomRight = vector_add_2i(canvasCenter, vector_div_2i(canvasSize, 2));
		


		set_quad_positions(topLeft, bottomRight);
		set_quad_corners(topLeftCorner, bottomRightCorner);


		glDrawArrays(GL_QUADS, 0, 4);
	}
}

void PT_IMAGELABEL_destroy(void* obj) {
	PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)obj;

	PT_GUI_OBJ_destroy((void*)imagelabel->guiObj);

	free(imagelabel);
}