#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"

#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "ScreenUI.h"
#include "scrollFrame.h"
#include "PeachTeaShaders.h"

PT_canvas update_gui_instance_size(Instance* instance, PT_canvas parentCanvas) {
	PT_canvas canvas;

	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		canvas = PT_GUI_OBJ_update_size((PT_GUI_OBJ*)instance->subInstance, parentCanvas);
		break;
	case IT_TEXTLABEL:
		canvas = PT_TEXTLABEL_update_size((PT_TEXTLABEL*)instance->subInstance, parentCanvas);
		break;
	case IT_IMAGELABEL:
		canvas = PT_IMAGELABEL_update_size((PT_IMAGELABEL*)instance->subInstance, parentCanvas);
		break;
	case IT_SCROLLFRAME:
		canvas = PT_SCROLLFRAME_update_size((PT_SCROLLFRAME*)instance->subInstance, parentCanvas);
		break;
	}

	return canvas;
}

void render_gui_instance(Instance* instance) {
	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		PT_GUI_OBJ_render((PT_GUI_OBJ*)instance->subInstance);
		break;
	case IT_TEXTLABEL:
		PT_TEXTLABEL_render((PT_TEXTLABEL*)instance->subInstance);
		break;
	case IT_IMAGELABEL:
		PT_IMAGELABEL_render((PT_IMAGELABEL*)instance->subInstance);
		break;
	case IT_SCROLLFRAME:
		PT_SCROLLFRAME_render((PT_SCROLLFRAME*)instance->subInstance);
		break;
	}
}
