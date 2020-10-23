#include <stdlib.h>
#include "textLabel.h"
#include "guiObj.h"
#include "guiUtil.h"

Instance* PT_TEXLABEL_new() {
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)calloc(1, sizeof(PT_TEXTLABEL));
	
	Instance* inst = PT_GUI_OBJ_new();
	textlabel->guiObj = (PT_GUI_OBJ*)inst->subInstance;
	inst->subInstance = (void*)textlabel;
	textlabel->instance = inst;

	inst->destroySubInstance = PT_TEXTLABEL_destroy;
	inst->instanceType = IT_TEXTLABEL;

	return inst;
}

void PT_TEXTLABEL_destroy(void* obj) {
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)obj;

	PT_GUI_OBJ_destroy((void*)textlabel->guiObj);

	free(textlabel);
}

PT_GUI_DIMS PT_TEXTLABEL_render(PT_TEXTLABEL* textlabel, PT_GUI_DIMS parentDims) {
	PT_GUI_DIMS childDims = PT_GUI_OBJ_render(textlabel->guiObj, parentDims);

	return childDims;
}