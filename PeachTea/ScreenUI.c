#include "screenUI.h"

Instance* PT_SCREEN_UI_new() {
	Instance* instance = new_instance();
	instance->instanceType = IT_SCREEN_UI;

	return instance;
}

void PT_SCREEN_UI_destroy(void* obj) {
	PT_SCREEN_UI* screenUI = (PT_SCREEN_UI*)obj;
	free(screenUI);
}

PT_GUI_DIMS PT_SCREEN_UI_render(PT_SCREEN_UI* ui, PT_GUI_DIMS parentDims) {
	return parentDims;
}