#include "renderFrame.h"
#include "vectorMath.h"
#include "PeachTeaShaders.h"
#include "glExtensions.h"

#include <stdlib.h>
#include <string.h>

Instance* PT_RENDERFRAME_new() {
	PT_RENDERFRAME* renderFrame = calloc(1, sizeof(PT_RENDERFRAME));
	renderFrame->visible = 1;

	Instance* instance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;

	renderFrame->guiObj = obj;
	renderFrame->instance = instance;
	instance->subInstance = (void*)renderFrame;
	instance->instanceType = IT_RENDERFRAME;

	return instance;
}

PT_RENDERFRAME* PT_RENDERFRAME_clone(PT_RENDERFRAME* source, Instance* instance) {
	PT_RENDERFRAME* clone = calloc(1, sizeof(PT_RENDERFRAME));

	memcpy(clone, source, sizeof(PT_RENDERFRAME));
	clone->instance = instance;
	clone->guiObj = PT_GUI_OBJ_clone(source->guiObj, instance);

	return clone;
}

void PT_RENDERFRAME_destroy(void* obj) {
	PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)obj;

	PT_GUI_OBJ_destroy((void*)renderFrame->guiObj);

	free(renderFrame);
}

PT_canvas PT_RENDERFRAME_update_size(PT_RENDERFRAME* renderFrame, PT_canvas parentCanvas) {
	PT_canvas lastCanvas = renderFrame->guiObj->lastCanvas;
	PT_canvas thisCanvas = PT_GUI_OBJ_update_size(renderFrame->guiObj, parentCanvas);

	vec2i lastCanvasSize = canvas_size(lastCanvas);
	vec2i thisCanvasSize = canvas_size(thisCanvas);

	if (thisCanvasSize.x > 0 && thisCanvasSize.y > 0) {
		if (!vector_equal_2i(lastCanvasSize, thisCanvasSize) && renderFrame->renderTexture.tex) {
			renderFrame->renderTexture = PT_FRAMETEXTURE_resize(renderFrame->renderTexture, thisCanvasSize.x, thisCanvasSize.y);
		}
		else if (!renderFrame->renderTexture.tex) {
			renderFrame->renderTexture = PT_FRAMETEXTURE_new(thisCanvasSize.x, thisCanvasSize.y, 0);
		}
	}

	return thisCanvas;
}
void PT_RENDERFRAME_render(PT_RENDERFRAME* renderFrame, PT_SCREEN_UI* ui) {
	renderFrame->guiObj->visible = renderFrame->visible;
	PT_canvas childCanvas = renderFrame->guiObj->lastCanvas;

	vec2i pos = canvas_pos(childCanvas);
	vec2i size = canvas_size(childCanvas);

	if (renderFrame->visible) {
		if (renderFrame->render) {
			if (renderFrame->renderTexture.tex) {
				PT_FRAMETEXTURE_bind(renderFrame->renderTexture);

				PT_FRAMETEXTURE_clear(renderFrame->renderTexture);

				glDepthFunc(GL_LESS);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				renderFrame->render(renderFrame); // call user-defined render function
			}
		}

		PT_FRAMETEXTURE_bind(ui->frameTexture);
		PT_GUI_OBJ_render(renderFrame->guiObj, ui);

		//printf("%s\n", renderFrame->instance->name);
		if (renderFrame->renderTexture.tex) {
			// copy user-rendered texture to window
			glUseProgram(PTS_tex);
			

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderFrame->renderTexture.tex);

			set_quad_positions(pos, vector_add_2i(pos, size));
			set_quad_corners(
				(vec2f){0, 1}, 
				(vec2f) { 1, 0 }
			);

			glDrawArrays(GL_QUADS, 0, 4);
		}
	}
}



