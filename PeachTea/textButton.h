#ifndef PT_TEXTBUTTON_H
#define PT_TEXTBUTTON_H

#include "guiObj.h"
#include "textLabel.h"
#include "Instance.h"
#include "ScreenDimension.h"

typedef struct {
	Instance* instance;
	PT_TEXTLABEL* textlabel;
	PT_GUI_OBJ* guiObj; // points to the same guiObj that the textlabel does, just a shortcut

	int active;
	int visible;

	void (*onActivate)(void);
} PT_TEXTBUTTON;

Instance* PT_TEXTBUTTON_new();
PT_ABS_DIM PT_TEXTBUTTON_render(PT_TEXTBUTTON* textbutton, PT_ABS_DIM parentDims);
void PT_TEXTBUTTON_destroy(void* obj);

#endif