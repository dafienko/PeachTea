#include "textButton.h"

Instance* PT_TEXTBUTTON_new() {
	PT_TEXTBUTTON* textbutton = calloc(1, sizeof(PT_TEXTBUTTON));

	textbutton->active = 1;

	Instance* instance = PT_TEXTLABEL_new();
	PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)instance->subInstance;
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)textlabel->guiObj;
	
	textbutton->guiObj = guiObj;
	textbutton->textlabel = textlabel;

	instance->subInstance = textbutton;

	return instance;
}

PT_ABS_DIM PT_TEXTBUTTON_render(PT_TEXTBUTTON* textbutton, PT_ABS_DIM parentDims) {
	return PT_TEXTLABEL_render(textbutton->textlabel, parentDims);
}

void PT_TEXTBUTTON_destroy(void* obj) {
	PT_TEXTBUTTON* textbutton = (PT_TEXTBUTTON*)obj;
	PT_TEXTLABEL_destroy(textbutton->textlabel);
}