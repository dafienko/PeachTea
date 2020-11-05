#include "imageLabel.h"
#include <stdlib.h>

Instance* PT_IMAGELABEL_new() {
	PT_IMAGELABEL* imgLabel = calloc(1, sizeof(PT_IMAGELABEL));
	imgLabel->visible = 1;
	imgLabel->img.texId = -1;

	Instance* instance = PT_GUI_OBJ_new();
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;

	imgLabel->guiObj = obj;
	imgLabel->instance = instance;
	instance->subInstance = (void*)imgLabel;
	instance->instanceType = IT_IMAGELABEL;

	return instance;
}

PT_ABS_DIM PT_IMAGELABEL_render(PT_IMAGELABEL* img, PT_ABS_DIM parentDims) {
	PT_ABS_DIM childDims = PT_GUI_OBJ_render(img->guiObj, parentDims);



	return childDims;
}

void PT_IMAGELABEL_destroy(void* imgLabel) {

}