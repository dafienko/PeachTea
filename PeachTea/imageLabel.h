#ifndef PT_IMAGE_LABEL_H
#define PT_IMAGE_LABEL_H

#include "Instance.h"
#include "guiObj.h"
#include "imageLoader.h"

typedef struct {
	Instance* instance;
	PT_GUI_OBJ* guiObj;
	int visible;

	PT_IMAGE img;


} PT_IMAGELABEL;

Instance* PT_IMAGELABEL_new();

PT_ABS_DIM PT_IMAGELABEL_render(PT_IMAGELABEL* img, PT_ABS_DIM parentDims);

void PT_IMAGELABEL_destroy(void* imgLabel);


#endif