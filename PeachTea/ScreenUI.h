#ifndef PT_SCREEN_UI_H
#define PT_SCREEN_UI_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "vectorMath.h"
#include "Instance.h"
#include "frameTexture.h"
#include "canvas.h"
#include "bool.h"
#include "ScreenDimension.h"

typedef enum {
	ZST_SIBLING,
	ZST_GLOBAL
} Z_SORTING_TYPE;

typedef enum {
	PT_H_ALIGNMENT_LEFT,
	PT_H_ALIGNMENT_CENTER,
	PT_H_ALIGNMENT_RIGHT
} TEXT_HORIZONTAL_ALIGNMENT;

typedef enum {
	PT_V_ALIGNMENT_TOP,
	PT_V_ALIGNMENT_CENTER,
	PT_V_ALIGNMENT_BOTTOM
} TEXT_VERTICAL_ALIGNMENT;

typedef enum {
	PT_FONT_COMIC,

	PT_FONT_CONSOLA,
	PT_FONT_CONSOLA_B,

	PT_FONT_ARIAL,
	PT_FONT_ARIAL_B,
	PT_FONT_ARIAL_BLK,
	PT_FONT_ARIAL_BI,
	PT_FONT_ARIAL_I,

	PT_FONT_TIMES
} PT_FONT;

typedef struct {
	Instance* instance;

	Z_SORTING_TYPE sortingType;

	PT_FRAMETEXTURE frameTexture;
	PT_FRAMETEXTURE effectTexture1;
	PT_FRAMETEXTURE effectTexture2;
	
	int lastBlurZindex;
	Instance* lastRootInstance;

	struct PT_UI_RENDER_TREE* lastRenderTree;

	BOOL enabled;
} PT_SCREEN_UI;

#include <Windows.h>

void PT_SCREEN_UI_init();

Instance* PT_SCREEN_UI_new();
PT_SCREEN_UI* PT_SCREEN_UI_clone(PT_SCREEN_UI* source, Instance* instanceClone);
void PT_SCREEN_UI_destroy(void* obj);

PT_canvas PT_SCREEN_UI_render(PT_SCREEN_UI* ui);

struct PT_GUI_OBJ* get_instance_gui_obj(Instance* instance);

PT_canvas update_gui_instance_size(Instance* instance, PT_canvas parentCanvas);
void render_gui_instance(Instance* instance, PT_SCREEN_UI* ui);

void PT_SCREEN_UI_update_rendertree(PT_SCREEN_UI* ui);

void set_quad_positions(vec2i topLeft, vec2i bottomRight);
void set_quad_corners(vec2f topLeft, vec2f bottomRight);
void default_quad_corners();

void PT_SCREEN_UI_update_blur_tex(PT_SCREEN_UI* ui, struct PT_GUI_OBJ* obj);

#endif