#ifndef PT_RENDERFRAME_H
#define PT_RENDERFRAME_H

#include "Instance.h"
#include "guiObj.h"
#include "ScreenUI.h"
#include "frameTexture.h"

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj;

	int visible;

	PT_FRAMETEXTURE renderTexture;
	void (*render)(struct PT_RENDERFRAME*);
} PT_RENDERFRAME;

void PT_RENDERFRAME_render(PT_RENDERFRAME* obj, PT_SCREEN_UI* ui);
PT_canvas PT_RENDERFRAME_update_size(PT_RENDERFRAME* obj, PT_canvas parentCanvas);

void PT_RENDERFRAME_destroy(void* obj);

Instance* PT_RENDERFRAME_new();
PT_RENDERFRAME* PT_RENDERFRAME_clone(PT_RENDERFRAME* source, Instance* instance);

#endif