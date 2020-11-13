#include "screenUI.h"
#include <stdlib.h>
#include "guiObj.h"
#include "mouse.h"
#include "textLabel.h"

#include <stdio.h>

PT_SCREEN_UI** screenUIs;
int numScreenUIs;
int screenUIsRoom;

void add_screen_ui(PT_SCREEN_UI* ui) {
	if (screenUIs == NULL) {
		screenUIsRoom = 2;
		screenUIs = calloc(screenUIsRoom, sizeof(PT_SCREEN_UI*));
	}

	if (numScreenUIs + 1 > screenUIsRoom) {
		screenUIsRoom *= 2;
		screenUIs = realloc(screenUIs, screenUIsRoom * sizeof(PT_SCREEN_UI*));
	}

	*(screenUIs + numScreenUIs) = ui;
	numScreenUIs++;
}

int pos_in_obj(vec2i pos, PT_GUI_OBJ* obj) {
	static int left, right, top, bottom;

	left = obj->lastAbsoluteDim.position.x;
	right = left + obj->lastAbsoluteDim.size.x;
	top = obj->lastAbsoluteDim.position.y;
	bottom = top + obj->lastAbsoluteDim.size.y;

	return pos.x > left && pos.x < right&& pos.y > top && pos.y < bottom;
}

void obj_mouse_moved(PT_GUI_OBJ* obj) {
	int mouseWasInObj = obj->mouseInFrame;
	obj->mouseInFrame = pos_in_obj(mousePos, obj);

	if (mouseWasInObj != obj->mouseInFrame) {
		if (obj->mouseInFrame) {
			//printf("mouse entered %s\n", obj->instance->name);

			PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseEnter, obj);
		}
		else {
			//printf("mouse left %s\n", obj->instance->name);

			PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseLeave, obj);
		}
	}
}

void obj_mouse_down(PT_GUI_OBJ* obj) {
	if (pos_in_obj(mousePos, obj)) {
		obj->pressed = 1;

		//printf("pressed %s\n", obj->instance->name);

		PT_BINDABLE_EVENT_fire(&obj->e_obj_pressed, obj);
	}
}

void obj_mouse_up(PT_GUI_OBJ* obj) {
	if (obj->pressed) {
		obj->pressed = 0;
		//printf("released %s\n", obj->instance->name);

		PT_BINDABLE_EVENT_fire(&obj->e_obj_released, obj);

		if (pos_in_obj(mousePos, obj)) {
			PT_BINDABLE_EVENT_fire(&obj->e_obj_activated, obj);
		}
	}
}

void enumerate_gui_objs(Instance* parent,  void(*callback)(PT_GUI_OBJ*)) {
	if (parent == NULL) {
		for (int i = 0; i < numScreenUIs; i++) {
			PT_SCREEN_UI* ui = *(screenUIs + i);
			Instance* p = ui->instance;
			enumerate_gui_objs(p, callback);
		}
	}
	else {
		INSTANCE_TYPE it = parent->instanceType;

		PT_GUI_OBJ* obj = NULL;

		switch (it) {
		case IT_GUI_OBJ:
			obj = (PT_GUI_OBJ*)parent->subInstance;
			break;
		case IT_TEXTLABEL:
			;
			PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)parent->subInstance;
			obj = textlabel->guiObj;
			break;
		}

		if (obj != NULL) {
			callback(obj);
		}

		for (int i = 0; i < parent->numChildren; i++) {
			Instance* child = *(parent->children + i);
			enumerate_gui_objs(child, callback);
		}
	}
}

void on_mouse_move(void* args) {
	enumerate_gui_objs(NULL, obj_mouse_moved);
}

void on_mouse_up(void* args) {
	enumerate_gui_objs(NULL, obj_mouse_up);
}

void on_mouse_down(void* args) {
	printf("mouse down\n");
	enumerate_gui_objs(NULL, obj_mouse_down);
}

void PT_SCREEN_UI_init() {
	PT_BINDABLE_EVENT_bind(&e_mouseMove, on_mouse_move);
	PT_BINDABLE_EVENT_bind(&e_mouseUp, on_mouse_up);
	PT_BINDABLE_EVENT_bind(&e_mouseDown, on_mouse_down);
}

Instance* PT_SCREEN_UI_new() {
	PT_SCREEN_UI* ui = calloc(1, sizeof(PT_SCREEN_UI));
	ui->enabled = 1;
	
	Instance* instance = new_instance();
	instance->instanceType = IT_SCREEN_UI;
	instance->subInstance = (void*)ui;

	ui->instance = instance;

	add_screen_ui(ui);

	return instance;
}

PT_SCREEN_UI* PT_SCREEN_UI_clone(PT_SCREEN_UI* source, Instance* instanceClone) {
	PT_SCREEN_UI* clone = calloc(1, sizeof(PT_SCREEN_UI));
	memcpy(clone, source, sizeof(PT_SCREEN_UI));

	clone->instance = instanceClone;

	add_screen_ui(clone);

	return clone;
}

void PT_SCREEN_UI_destroy(void* obj) {
	PT_SCREEN_UI* screenUI = (PT_SCREEN_UI*)obj;

	int removeIndex = -1;
	for (int i = 0; i < numScreenUIs; i++) {
		PT_SCREEN_UI* ui = *(screenUIs + i);
		if (ui == screenUI) {
			removeIndex = i;
			break;
		}
	}

	if (removeIndex > 0) {
		*(screenUIs + removeIndex) = NULL;
		numScreenUIs--;

		if (removeIndex < numScreenUIs) {
			*(screenUIs + removeIndex) = *(screenUIs + numScreenUIs);
			*(screenUIs + numScreenUIs) = NULL;
		}
	}

	free(screenUI);
}

PT_ABS_DIM PT_SCREEN_UI_render(PT_SCREEN_UI* ui, PT_ABS_DIM parentDims) {
	return parentDims;
}