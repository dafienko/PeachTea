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
#include "renderFrame.h"

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

int obj_wheel_scroll(PT_GUI_OBJ* obj, int d) {
	if (pos_in_obj(mousePos, obj)) {
		Instance* instance = obj->instance;
		
		if (instance->instanceType == IT_SCROLLFRAME) {
			PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)instance->subInstance;
			int topBound = 0;

			PT_canvas lastCanvas = scrollframe->guiObj->lastCanvas;
			vec2i size = canvas_size(lastCanvas);

			vec2i scrollFrameCanvasSize = calculate_screen_dimension(scrollframe->canvasSize, size);
			
			int bottomBound = max(0, scrollFrameCanvasSize.y - size.y);

			int currentY = scrollframe->canvasPosition.y;

			scrollframe->canvasPosition.y = clamp(currentY + d, topBound, bottomBound);
		}

		return 1;
	}	

	return 0;
}

int obj_scroll_up(PT_GUI_OBJ* obj) {
	return obj_wheel_scroll(obj, -50);
}

int obj_scroll_down(PT_GUI_OBJ* obj) {
	return obj_wheel_scroll(obj, 50);
}

int obj_mouse_moved(PT_GUI_OBJ* obj) {
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

	return 0;
}

int obj_mouse1_down(PT_GUI_OBJ* obj) {
	if (pos_in_obj(mousePos, obj)) {
		obj->pressed = 1;

		PT_BINDABLE_EVENT_fire(&obj->e_obj_pressed, obj);

		return 1;
	}

	return 0;
}

int obj_mouse1_up(PT_GUI_OBJ* obj) {
	if (obj->pressed) {
		obj->pressed = 0;

		PT_BINDABLE_EVENT_fire(&obj->e_obj_released, obj);

		if (pos_in_obj(mousePos, obj)) {
			PT_BINDABLE_EVENT_fire(&obj->e_obj_activated, obj);
			return 1;
		}
	}

	return 0;
}

int process_callback(Instance* instance, int(*callback)(PT_GUI_OBJ*)) {
	INSTANCE_TYPE it = instance->instanceType;

	PT_GUI_OBJ* obj = NULL;

	int processed = 0;

	switch (it) {
	case IT_GUI_OBJ:
		obj = (PT_GUI_OBJ*)instance->subInstance;
		break;
	case IT_TEXTLABEL:
		;
		PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)instance->subInstance;
		obj = textlabel->guiObj;
		break;
	case IT_IMAGELABEL:
		;
		PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)instance->subInstance;
		obj = imageLabel->guiObj;
		break;
	case IT_SCROLLFRAME:
		;
		PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)instance->subInstance;
		obj = scrollFrame->guiObj;
		break;
	case IT_RENDERFRAME:
		;
		PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)instance->subInstance;
		obj = renderFrame->guiObj;
		break;
	}

	if (!processed && obj != NULL) {
		if (obj->visible && obj->processEvents) {
			processed = callback(obj);
		}
	}

	return processed;
}

int enumerate_render_tree(PT_UI_RENDER_TREE* renderTree, PT_SCREEN_UI* ui, int(*callback)(PT_GUI_OBJ*)) {
	int processed = 0;

	Instance* rootInstance = renderTree->rootInstance;

	int isScrollEvent = callback == obj_scroll_down || callback == obj_scroll_up;

	// process scrollframe controls
	if (!processed && rootInstance->instanceType == IT_SCROLLFRAME) {
		PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)rootInstance->subInstance;
		processed = callback(scrollFrame->vscrollTrack);
		processed = callback(scrollFrame->vscrollBar);

		processed = callback(scrollFrame->hscrollTrack);
		processed = callback(scrollFrame->hscrollBar);

		if (isScrollEvent) {
			process_callback(rootInstance, callback);
		}
	}

	if (!processed) {
		for (int i = renderTree->numBranches - 1; i >= 0; i--) {
			PT_UI_RENDER_TREE* branch = *(renderTree->branches + i);
			processed = enumerate_render_tree(branch, ui, callback);
			if (processed) {
				return processed; // once an event has been processed once, stop enumerating over instances
			}
		}
	}


	
	if (!processed && IS_UI_INSTANCE(rootInstance->instanceType)) {
		if (!(rootInstance->instanceType == IT_SCROLLFRAME && isScrollEvent)) { // if this is a scroll event and the root instance is a scroll frame, the event has already been processed
			processed = process_callback(rootInstance, callback);
		}
	}

	

	return processed;
}


int enumerate_screeenuis(int(*callback)(PT_GUI_OBJ*)) {
	int processed = 0;

	for (int i = 0; i < numScreenUIs; i++) {
		PT_SCREEN_UI* ui = *(screenUIs + i);
		Instance* p = ui->instance;

		PT_UI_RENDER_TREE* renderTree = ui->lastRenderTree;
		if (renderTree) {
			processed = enumerate_render_tree(renderTree, ui, callback);

			if (processed) {
				break;
			}
		}
	}
}

void on_mouse_move(void* args) {
	enumerate_screeenuis(obj_mouse_moved);
}

void on_mouse_up(void* args) {
	enumerate_screeenuis(obj_mouse1_up);
}

void on_mouse_down(void* args) {
	enumerate_screeenuis(obj_mouse1_down);
}

void on_wheel_scroll_up(void* args) {
	enumerate_screeenuis(obj_scroll_up);
}

void on_wheel_scroll_down(void* args) {
	enumerate_screeenuis(obj_scroll_down);
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
	ui->frameTexture = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);
	ui->effectTexture2 = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);
	ui->effectTexture1 = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);

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
	clone->frameTexture = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);

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

	if (ui->lastRenderTree) {
		PT_UI_RENDER_TREE_destroy(ui->lastRenderTree);
	}

	PT_UI_RENDER_TREE* tree = PT_UI_RENDER_TREE_generate(ui);

	PT_UI_RENDER_TREE_render(tree, ui);
	PT_FRAMETEXTURE_copy_to_framebuffer(ui->frameTexture, 0);	

	ui->lastRenderTree = tree;

	return canvas;
}