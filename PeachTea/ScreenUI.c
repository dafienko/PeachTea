#include "screenUI.h"
#include <stdlib.h>
#include "guiObj.h"
#include "mouse.h"
#include "textLabel.h"
#include "imageLabel.h"
#include "screenSize.h"
#include "scrollFrame.h"
#include "clamp.h"
#include "glExtensions.h"
#include "uiRenderTree.h"
#include "frameTexture.h"

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

void obj_wheel_scroll(PT_GUI_OBJ* obj, int d) {
	if (obj->visible && pos_in_obj(mousePos, obj)) {
		Instance* instance = obj->instance;
		
		if (instance->instanceType == IT_SCROLLFRAME) {
			PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)instance->subInstance;
			int topBound = 0;
			vec2i canvasSize = calculate_screen_dimension(scrollframe->canvasSize, obj->lastAbsoluteDim.size);
			int bottomBound = canvasSize.y - obj->lastAbsoluteDim.size.y;

			int currentY = scrollframe->canvasPosition.y;

			scrollframe->canvasPosition.y = clamp(currentY + d, topBound, bottomBound);
		}
	}	
}

void obj_scroll_up(PT_GUI_OBJ* obj) {
	obj_wheel_scroll(obj, -50);
}

void obj_scroll_down(PT_GUI_OBJ* obj) {
	obj_wheel_scroll(obj, 50);
}

void obj_mouse_moved(PT_GUI_OBJ* obj) {
	if (obj->visible) {
		int mouseWasInObj = obj->mouseInFrame;
		obj->mouseInFrame = pos_in_obj(mousePos, obj);

		if (mouseWasInObj != obj->mouseInFrame) {
			if (obj->mouseInFrame) {
				PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseEnter, obj);
			}
			else {
				PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseLeave, obj);
			}
		}

		if (obj->pressed) {
			PT_BINDABLE_EVENT_fire(&obj->e_obj_dragged, obj);
		}
	}
}

void obj_mouse1_down(PT_GUI_OBJ* obj) {
	if (obj->visible && pos_in_obj(mousePos, obj)) {
		obj->pressed = 1;

		PT_BINDABLE_EVENT_fire(&obj->e_obj_pressed, obj);
	}
}

void obj_mouse1_up(PT_GUI_OBJ* obj) {
	if (obj->pressed) {
		obj->pressed = 0;

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
		case IT_IMAGELABEL:
			;
			PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)parent->subInstance;
			obj = imageLabel->guiObj;
			break;
		case IT_SCROLLFRAME:
			;
			PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)parent->subInstance;
			obj = scrollFrame->guiObj;

			callback(scrollFrame->vscrollBar);
			callback(scrollFrame->hscrollBar);

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
	enumerate_gui_objs(NULL, obj_mouse1_up);
}

void on_mouse_down(void* args) {
	enumerate_gui_objs(NULL, obj_mouse1_down);
}

void on_wheel_scroll_up(void* args) {
	enumerate_gui_objs(NULL, obj_scroll_up);
}

void on_wheel_scroll_down(void* args) {
	enumerate_gui_objs(NULL, obj_scroll_down);
}

void PT_SCREEN_UI_init() {
	PT_BINDABLE_EVENT_bind(&e_mouseMove, on_mouse_move);
	PT_BINDABLE_EVENT_bind(&e_mouse1Up, on_mouse_up);
	PT_BINDABLE_EVENT_bind(&e_mouse1Down, on_mouse_down);
	PT_BINDABLE_EVENT_bind(&e_wheelUp, on_wheel_scroll_up);
	PT_BINDABLE_EVENT_bind(&e_wheelDown, on_wheel_scroll_down);
}

Instance* PT_SCREEN_UI_new() {
	PT_SCREEN_UI* ui = calloc(1, sizeof(PT_SCREEN_UI));
	ui->enabled = 1;
	ui->frameTexture = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y);
	ui->effectTexture1 = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y);
	ui->effectTexture2 = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y);

	PT_FRAMETEXTURE_bind_to_screensize(&ui->frameTexture);
	PT_FRAMETEXTURE_bind_to_screensize(&ui->effectTexture1);
	PT_FRAMETEXTURE_bind_to_screensize(&ui->effectTexture2);

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
	clone->frameTexture = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y);

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

void update_instance_size_recur(Instance* instance, PT_canvas parentCanvas) {
	PT_canvas childCanvas = parentCanvas;
	if (IS_UI_INSTANCE(instance->instanceType)) {
		childCanvas = update_gui_instance_size(instance, parentCanvas);
	}

	const char* defName = "instance";
	char* name = instance->name;
	int shouldFreeName = 0;
	if (name == NULL) {
		shouldFreeName = 1;
		name = calloc(strlen(defName) + 1, sizeof(char));
		memcpy(name, defName, (strlen(defName)) * sizeof(char));
	}

	if (shouldFreeName) {
		free(name);
	}

	for (int i = 0; i < instance->numChildren; i++) {
		Instance* child = *(instance->children + i);
		update_instance_size_recur(child, childCanvas);
	}
}

PT_canvas PT_SCREEN_UI_render(PT_SCREEN_UI* ui) {
	PT_canvas canvas = { 0 };

	canvas.right = screenSize.x;
	canvas.bottom = screenSize.y;

	update_instance_size_recur(ui->instance, canvas);


	PT_FRAMETEXTURE_clear(ui->frameTexture);
	PT_FRAMETEXTURE_bind(ui->frameTexture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	PT_UI_RENDER_TREE* tree = PT_UI_RENDER_TREE_generate(ui);

	PT_UI_RENDER_TREE_render(tree, ui);
	PT_FRAMETEXTURE_copy_to_framebuffer(ui->frameTexture, 0);	

	PT_UI_RENDER_TREE_destroy(tree);

	return canvas;
}