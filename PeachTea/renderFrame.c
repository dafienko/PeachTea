#include "renderFrame.h"

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
	instance->instanceType = IT_IMAGELABEL;

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

	PT_FRAMETEXTURE newTexture;

	vec2i thisCanvasSize = canvas_size(thisCanvas);
	if (!vector_equal_2i(canvas_size(lastCanvas), thisCanvasSize) && renderFrame->renderTexture.tex) {
		newTexture = PT_FRAMETEXTURE_resize(renderFrame->renderTexture, thisCanvasSize.x, thisCanvasSize.y);
	}
	else {
		newTexture = PT_FRAMETEXTURE_new(thisCanvasSize.x, thisCanvasSize.y);
	}

	renderFrame->renderTexture = newTexture;

	return thisCanvas;
}
void PT_RENDERFRAME_render(PT_RENDERFRAME* renderFrame, PT_SCREEN_UI* ui) {
	renderFrame->guiObj->visible = renderFrame->visible;
	PT_canvas childCanvas = renderFrame->guiObj->lastCanvas;

	PT_GUI_OBJ_render(renderFrame->guiObj, ui);
}



