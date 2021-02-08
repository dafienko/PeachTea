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
#include "expandableArray.h"

#include <stdio.h>

PT_EXPANDABLE_ARRAY screenUIs = { 0 };

int pos_in_obj(vec2i pos, PT_GUI_OBJ* obj) {
	static int left, right, top, bottom;

	left = obj->lastAbsoluteDim.position.x;
	right = left + obj->lastAbsoluteDim.size.x;
	top = obj->lastAbsoluteDim.position.y;
	bottom = top + obj->lastAbsoluteDim.size.y;

	return pos.x > left && pos.x < right && pos.y > top && pos.y < bottom;
}

int obj_wheel_scroll(PT_GUI_OBJ* obj, int d, int processed) {
	if (!processed && pos_in_obj(mousePos, obj)) {
		Instance* instance = obj->instance;
		
		if (instance && instance->instanceType == IT_SCROLLFRAME) {
			PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)instance->subInstance;
			int topBound = 0;

			PT_canvas lastCanvas = scrollframe->guiObj->lastCanvas;
			vec2i size = canvas_size(lastCanvas);

			vec2i scrollFrameCanvasSize = calculate_screen_dimension(scrollframe->canvasSize, size);
			
			int bottomBound = max(0, scrollFrameCanvasSize.y - size.y);

			int currentY = scrollframe->targetCanvasPosition.y;

			scrollframe->targetCanvasPosition.y = clamp(currentY + d * .5f, topBound, bottomBound);
		}

		return 1;
	}	

	return 0;
}

int obj_scroll_up(PT_GUI_OBJ* obj, int processed) {
	return obj_wheel_scroll(obj, get_mousewheel_delta(), processed);
}

int obj_scroll_down(PT_GUI_OBJ* obj, int processed) {
	return obj_wheel_scroll(obj, get_mousewheel_delta(), processed);
}

int obj_mouse_moved(PT_GUI_OBJ* obj, int processed) {
	int mouseWasInObj = obj->mouseInFrame;
	obj->mouseInFrame = pos_in_obj(mousePos, obj) && !processed;

	int thisProcessed = obj->mouseInFrame;

	if (obj->pressed && !processed) {
		vec2i delta = vector_sub_2i(mousePos, obj->pressedAt);
		if (delta.x * delta.x + delta.y * delta.y >= 3 * 3) { // mouse should move at least three pixels before being dragged
			PT_BINDABLE_EVENT_fire(&obj->e_obj_dragged, obj);
		}
	}

	if (mouseWasInObj != obj->mouseInFrame) {
		if (obj->mouseInFrame) {
			PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseEnter, obj);
		}
		else {
			PT_BINDABLE_EVENT_fire(&obj->e_obj_mouseLeave, obj);
		}
	}

	return thisProcessed | processed;
}

int obj_mouse1_down(PT_GUI_OBJ* obj, int processed) {
	if (pos_in_obj(mousePos, obj) && !processed) {
		obj->pressed = 1;

		obj->pressedAt = mousePos;

		PT_BINDABLE_EVENT_fire(&obj->e_obj_pressed, obj);

		return 1;
	}

	return 0;
}

int obj_mouse1_up(PT_GUI_OBJ* obj, int processed) {
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


/*
@param processed:
	input events are processed such that ui elements at the top of the screen run the callback first. This 
	allows click events to only be processed once at the top-most button, rather than firing every single 
	button that bounds the cursor. Callbacks that process events that should only be processed once (such
	as a mouse click) should return 1 if they handle the event, and only process the event if the processed
	argument was 0. 
*/
int process_callback(Instance* instance, int(*callback)(PT_GUI_OBJ*, int), int processed) {
	INSTANCE_TYPE it = instance->instanceType;

	PT_GUI_OBJ* obj = NULL;

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

	if (!obj->processEvents) {
		if (!(it == IT_SCROLLFRAME && (callback == obj_scroll_up || callback == obj_scroll_down))) {
			return processed;
		}
	}

	if (obj != NULL) {
		if (obj->visible) {
			processed |= callback(obj, processed);
		}
	}

	return processed;
}

int enumerate_render_tree(PT_UI_RENDER_TREE* renderTree, PT_SCREEN_UI* ui, int(*callback)(PT_GUI_OBJ*, int), int processed) {
	Instance* rootInstance = renderTree->rootInstance;

	int isScrollEvent = callback == obj_scroll_down || callback == obj_scroll_up;

	// process scrollframe controls
	if (rootInstance->instanceType == IT_SCROLLFRAME) {
		PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)rootInstance->subInstance;

		processed |= callback(scrollFrame->vscrollBar, processed);
		processed |= callback(scrollFrame->hscrollBar, processed);

		if (!processed && isScrollEvent) {
			processed |= process_callback(rootInstance, callback, processed);
		}
	}

	for (int i = renderTree->numBranches - 1; i >= 0; i--) {
		PT_UI_RENDER_TREE* branch = *(renderTree->branches + i);
		processed |= enumerate_render_tree(branch, ui, callback, processed);
	}
	


	if (IS_UI_INSTANCE(rootInstance->instanceType)) {
		if (!(rootInstance->instanceType == IT_SCROLLFRAME && isScrollEvent)) { // if this is a scroll event and the root instance is a scroll frame, the event has already been processed
			processed |= process_callback(rootInstance, callback, processed);
		}
	}

	

	return processed;
}


int enumerate_screeenuis(int(*callback)(PT_GUI_OBJ*, int)) {
	int processed = 0;

	for (int i = 0; i < screenUIs.numElements; i++) {
		PT_SCREEN_UI* ui = *(PT_SCREEN_UI**)PT_EXPANDABLE_ARRAY_get(&screenUIs, i);
		Instance* p = ui->instance;

		PT_UI_RENDER_TREE* renderTree = ui->lastRenderTree;
		if (renderTree) {
			processed |= enumerate_render_tree(renderTree, ui, callback, processed);
		}
	}

	return processed;
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

	if (screenUIs.elementSpace == 0) {
		screenUIs = PT_EXPANDABLE_ARRAY_new(5, sizeof(PT_SCREEN_UI*));
	}
	PT_EXPANDABLE_ARRAY_add(&screenUIs, &ui);

	return instance;
}

PT_SCREEN_UI* PT_SCREEN_UI_clone(PT_SCREEN_UI* source, Instance* instanceClone) {
	PT_SCREEN_UI* clone = calloc(1, sizeof(PT_SCREEN_UI));
	memcpy(clone, source, sizeof(PT_SCREEN_UI));

	clone->instance = instanceClone;
	clone->frameTexture = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);

	PT_EXPANDABLE_ARRAY_add(&screenUIs, &clone);

	return clone;
}

void PT_SCREEN_UI_destroy(void* obj) {
	PT_SCREEN_UI* screenUI = (PT_SCREEN_UI*)obj;

	int removeIndex = -1;
	removeIndex = PT_EXPANDABLE_ARRAY_find(&screenUIs, &screenUI);
	if (removeIndex >= 0) {
		PT_EXPANDABLE_ARRAY_remove(&screenUIs, removeIndex);
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

void print_rendertree(PT_UI_RENDER_TREE* tree, int tabs) {
	char* str = calloc(500, sizeof(char));
	int strIndex = 0;

	for (int i = 0; i < tabs; i++) {
		*(str + strIndex) = '\t';
		strIndex++;
	}

	memcpy(str + strIndex, tree->rootInstance->name, strlen(tree->rootInstance->name));
	printf("%s\n", str);

	free(str);

	for (int i = 0; i < tree->numBranches; i++) {
		PT_UI_RENDER_TREE* branch = *(tree->branches + i);
		print_rendertree(branch, tabs + 1);
	}
}

void PT_SCREEN_UI_update_rendertree(PT_SCREEN_UI* ui) {
	if (ui->lastRenderTree) {
		PT_UI_RENDER_TREE_destroy(ui->lastRenderTree);
	}

	PT_UI_RENDER_TREE* tree = PT_UI_RENDER_TREE_generate(ui);
	//print_rendertree(tree, 0);
	ui->lastRenderTree = tree;
}

void PT_SCREEN_UI_update_blur_tex(PT_SCREEN_UI* ui, PT_GUI_OBJ* obj) {
	int updateRequired = 1;

	if (ui->sortingType == ZST_GLOBAL) {
		if (ui->lastBlurZindex == obj->zIndex) {
			updateRequired = 0;
		}
	}
	else if (ui->sortingType == ZST_SIBLING) {
		if (ui->lastBlurZindex == obj->zIndex && obj->instance && ui->lastRootInstance == obj->instance->parent) {
			updateRequired = 0;
		}
	}

	if (updateRequired) {
		PT_FRAMETEXTURE_blur(ui->frameTexture.tex, ui->effectTexture1, (vec2f) { 0, 1 }, obj->blurRadius, 0);
		PT_FRAMETEXTURE_blur(ui->effectTexture1.tex, ui->effectTexture2, (vec2f) { 1, 0 }, obj->blurRadius, 1);
		PT_FRAMETEXTURE_bind(ui->frameTexture);

		ui->lastBlurZindex = obj->zIndex;
		if (obj->instance) {
			ui->lastRootInstance = obj->instance->parent; 
		}
		else {
			ui->lastRootInstance = NULL;
		}
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
		PT_UI_RENDER_TREE* tree = ui->lastRenderTree;
		tree->lastBlurZIndex = -1;

		PT_UI_RENDER_TREE_render(tree, ui);
		PT_FRAMETEXTURE_copy_to_framebuffer(ui->frameTexture, 0);
	}
	

	return canvas;
}