#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"

#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "ScreenUI.h"
#include "PeachTeaShaders.h"

void render_gui_instance(Instance* instance, PT_ABS_DIM parentDims) {
	PT_ABS_DIM childDims;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		childDims = PT_GUI_OBJ_render((PT_GUI_OBJ*)instance->subInstance, parentDims);
		break;
	case IT_TEXTLABEL:
		childDims = PT_TEXTLABEL_render((PT_TEXTLABEL*)instance->subInstance, parentDims);
		break;
	case IT_IMAGELABEL:
		childDims = PT_IMAGELABEL_render((PT_IMAGELABEL*)instance->subInstance, parentDims);
		break;
	case IT_SCREEN_UI:
		;
		PT_SCREEN_UI* ui = (PT_SCREEN_UI*)instance->subInstance;
		
		if (!ui->enabled) {
			return;
		}

		childDims = PT_SCREEN_UI_render(ui, parentDims);
		break;
	}

	for (int i = 0; i < instance->numChildren; i++) {
		Instance* childInstance = *(instance->children + i);

		render_gui_instance(childInstance, childDims);
	}
}