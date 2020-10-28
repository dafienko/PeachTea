#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"

#include "guiObj.h"
#include "textLabel.h"
#include "ScreenUI.h"


void init_gui_util() {
	qVAO = calloc(1, sizeof(GLuint));
	qVBO = calloc(2, sizeof(GLuint));

	glGenVertexArrays(1, qVAO);
	glBindVertexArray(*qVAO);
	glGenBuffers(2, qVBO);

	int quadPositions[] = {
		0, 0,
		0, 10,
		10, 10,
		10, 0
	};

	float quadCorners[] = {
		0, 0,
		0, 1.0,
		1.0f, 1.0,
		1.0f, 0
	};

	glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners), quadCorners, GL_STATIC_DRAW);

	GLuint qvs = create_vertex_shader("shaders\\basicQuad.vs");
	GLuint qfs = create_fragment_shader("shaders\\UIframe.fs");
	GLuint quadShaders[] = { qvs, qfs };
	frameProg = create_program(quadShaders, 2);
}

void render_gui_instance(Instance* instance, PT_ABS_DIM parentDims) {
	PT_ABS_DIM childDims;
	
	switch (instance->instanceType) {
	case IT_GUI_OBJ:
		childDims = PT_GUI_OBJ_render((PT_GUI_OBJ*)instance->subInstance, parentDims);
		break;
	case IT_TEXTLABEL:
		childDims = PT_TEXTLABEL_render((PT_TEXTLABEL*)instance->subInstance, parentDims);
		break;
	case IT_SCREEN_UI:
		childDims = PT_SCREEN_UI_render((PT_SCREEN_UI*)instance->subInstance, parentDims);
		break;
	}

	for (int i = 0; i < instance->numChildren; i++) {
		Instance* childInstance = *(instance->children + i);

		render_gui_instance(childInstance, childDims);
	}
}