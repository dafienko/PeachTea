#ifndef PT_GUI_UTIL
#define PT_GUI_UTIL

#include "vectorMath.h"
#include "bool.h"

#include "Instance.h"
#include "vectorMath.h"

unsigned int* qVAO;
unsigned int* qVBO;

typedef struct _PT_GUI_OBJ {
	Instance* instance;
	
	vec2i pos_px;
	vec2f pos_percent;
	vec2i scale_px;
	vec2f scale_percent;
	BOOL visible;

	int borderWidth;
	float borderTransparancy;
	vec3i border_color;

	float transparency;
	vec3i background_color;
	
	void (*onMouseEnter)(void);
	void (*onMouseLeave)(void);

	void (*render)(vec2i screenSize); // should never actually be called by user, although its there just in case
	void* typeData; // varies based on the gui object type, contains data for that specific type. Half-assed encapsulation 
} PT_GUI_OBJ;

void init_gui_util();

Instance* new_frame();

void render_default_obj(PT_GUI_OBJ* obj);


#endif