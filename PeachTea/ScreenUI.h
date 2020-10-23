#ifndef PT_SCREEN_UI_H
#define PT_SCREEN_UI_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "vectorMath.h"
#include "Instance.h"
#include "guiUtil.h"

#include <Windows.h>

typedef struct {
	Instance* instance;
	
	BOOL enabled;
} PT_SCREEN_UI;

Instance* PT_SCREEN_UI_new();
void PT_SCREEN_UI_destroy(void* obj);

PT_GUI_DIMS PT_SCREEN_UI_render(PT_SCREEN_UI* ui, PT_GUI_DIMS parentDims);

#endif