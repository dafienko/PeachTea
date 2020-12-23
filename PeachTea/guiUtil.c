#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"

#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "ScreenUI.h"
#include "scrollFrame.h"
#include "PeachTeaShaders.h"

void render_gui_instance(Instance* instance, PT_canvas parentCanvas, Z_SORTING_TYPE sortingType) {
	PT_canvas childCanvas;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		childCanvas = PT_GUI_OBJ_render((PT_GUI_OBJ*)instance->subInstance, parentCanvas, sortingType, 1);
		break;
	case IT_TEXTLABEL:
		childCanvas = PT_TEXTLABEL_render((PT_TEXTLABEL*)instance->subInstance, parentCanvas, sortingType, 1);
		break;
	case IT_IMAGELABEL:
		childCanvas = PT_IMAGELABEL_render((PT_IMAGELABEL*)instance->subInstance, parentCanvas, sortingType, 1);
		break;
	case IT_SCROLLFRAME:
		childCanvas = PT_SCROLLFRAME_render((PT_IMAGELABEL*)instance->subInstance, parentCanvas, sortingType, 1);
		break;
	case IT_SCREEN_UI:
		;
		PT_SCREEN_UI* ui = (PT_SCREEN_UI*)instance->subInstance;

		if (!ui->enabled) {
			return;
		}

		childCanvas = PT_SCREEN_UI_render(ui);
		break;
	}
}

void render_gui_children(Instance* instance, PT_canvas childCanvas, Z_SORTING_TYPE sortingType) {
	for (int i = 0; i < instance->numChildren; i++) {
		Instance* childInstance = *(instance->children + i);

		render_gui_instance(childInstance, childCanvas, sortingType);
	}
}