#ifndef PT_IMAGE_LABEL_H
#define PT_IMAGE_LABEL_H

#include "Instance.h"
#include "guiObj.h"
#include "imageLoader.h"
#include "ScreenUI.h"

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj;
	int visible;

	PT_IMAGE image;

	float imageTransparency;
	PT_COLOR imageTint;
	float imageTintAlpha;

	int reactive;
	PT_COLOR activeBackgroundColor;
	vec2f activeBackgroundRange;
} PT_IMAGELABEL;

Instance* PT_IMAGELABEL_new();
PT_IMAGELABEL* PT_IMAGELABEL_clone(PT_IMAGELABEL* source, Instance* instanceClone);

PT_canvas PT_IMAGELABEL_update_size(PT_IMAGELABEL* imgLabel, PT_canvas parentCanvas);
void PT_IMAGELABEL_render(PT_IMAGELABEL* img, PT_SCREEN_UI* ui);

void PT_IMAGELABEL_destroy(void* imgLabel);


#endif