#include "glExtensions.h"
#include "screenSize.h"

#include "ScreenUI.h"
#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "scrollFrame.h"
#include "PeachTeaShaders.h"
#include "renderFrame.h"

PT_canvas update_gui_instance_size(Instance* instance, PT_canvas parentCanvas) {
	PT_canvas canvas;

	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		canvas = PT_GUI_OBJ_update_size((PT_GUI_OBJ*)instance->subInstance, parentCanvas);
		break;
	case IT_TEXTLABEL:
		canvas = PT_TEXTLABEL_update_size((PT_TEXTLABEL*)instance->subInstance, parentCanvas);
		break;
	case IT_IMAGELABEL:
		canvas = PT_IMAGELABEL_update_size((PT_IMAGELABEL*)instance->subInstance, parentCanvas);
		break;
	case IT_SCROLLFRAME:
		canvas = PT_SCROLLFRAME_update_size((PT_SCROLLFRAME*)instance->subInstance, parentCanvas);
		break;
	case IT_RENDERFRAME:
		canvas = PT_RENDERFRAME_update_size((PT_RENDERFRAME*)instance->subInstance, parentCanvas);
		break;
	}

	return canvas;
}

PT_canvas get_instance_canvas(Instance* instance) {
	PT_canvas canvas = { 0 };
	
	if (instance) {
		PT_GUI_OBJ* obj;

		switch (instance->instanceType) {
		case IT_SCREEN_UI:
			canvas.bottom = screenSize.y;
			canvas.right = screenSize.x;
			break;
		case IT_GUI_OBJ:
			;
			obj = (PT_GUI_OBJ*)instance->subInstance;
			canvas = obj->lastCanvas;
			break;
		case IT_SCROLLFRAME:
			;
			PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)instance->subInstance;
			obj = scrollframe->guiObj;
			canvas = obj->lastCanvas;
			break;
		case IT_IMAGELABEL:
			;
			PT_IMAGELABEL* imagelabel = (PT_IMAGELABEL*)instance->subInstance;
			obj = imagelabel->guiObj;
			canvas = obj->lastCanvas;
			break;
		case IT_TEXTLABEL:
			;
			PT_TEXTLABEL* textlabel = (PT_TEXTLABEL*)instance->subInstance;
			obj = textlabel->guiObj;
			canvas = obj->lastCanvas;
			break;
		case IT_RENDERFRAME:
			;
			PT_RENDERFRAME* renderFrame = (PT_RENDERFRAME*)instance->subInstance;
			obj = renderFrame->guiObj;
			canvas = obj->lastCanvas;
			break;
		default:
			return get_instance_canvas(instance->parent); 
		}
	}

	return canvas;
}

void render_gui_instance(Instance* instance, PT_SCREEN_UI* ui) {
	PT_canvas parentCanvas = get_instance_canvas(instance->parent);
	PT_canvas childCanvas = get_instance_canvas(instance);
	
	if (child_canvas_in_parent_canvas(childCanvas, parentCanvas) || child_canvas_in_parent_canvas(parentCanvas, childCanvas)) {
		switch (instance->instanceType) {
		case IT_GUI_OBJ:
			PT_GUI_OBJ_render((PT_GUI_OBJ*)instance->subInstance, ui);
			break;
		case IT_TEXTLABEL:
			PT_TEXTLABEL_render((PT_TEXTLABEL*)instance->subInstance, ui);
			break;
		case IT_IMAGELABEL:
			PT_IMAGELABEL_render((PT_IMAGELABEL*)instance->subInstance, ui);
			break;
		case IT_SCROLLFRAME:
			PT_SCROLLFRAME_render((PT_SCROLLFRAME*)instance->subInstance, ui);
			break;
		case IT_RENDERFRAME:
			PT_RENDERFRAME_render((PT_RENDERFRAME*)instance->subInstance, ui);
			break;
		}
	}
}

void set_quad_positions(vec2i topLeft, vec2i bottomRight) {
	glBindVertexArray(*qVAO);

	glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
	int quadPositions[] = {
		topLeft.x, topLeft.y,
		topLeft.x, bottomRight.y,
		bottomRight.x, bottomRight.y,
		bottomRight.x, topLeft.y
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, NULL);
}

void set_quad_corners(vec2f topLeft, vec2f bottomRight) {
	glBindVertexArray(*qVAO);

	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));

	float quadCorners[] = {
		topLeft.x, topLeft.y,
		topLeft.x, bottomRight.y,
		bottomRight.x, bottomRight.y,
		bottomRight.x, topLeft.y,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners), quadCorners, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void default_quad_corners() {
	glBindVertexArray(*qVAO);

	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glBufferData(GL_ARRAY_BUFFER, sizeof(DEFAULT_QUAD_CORNERS), DEFAULT_QUAD_CORNERS, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}