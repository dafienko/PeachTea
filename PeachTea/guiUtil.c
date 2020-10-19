#include "guiUtil.h"
#include "glExtensions.h"
#include "screenSize.h"

GLuint frameProg;

vec2i scale_and_offset_to_screen(vec2f scale, vec2i offset) {
	return (vec2i){
		(int)(scale.x * screenSize.x) + offset.x,
		(int)(scale.y * screenSize.y) + offset.y,
	};
}

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

void render_default_obj(PT_GUI_OBJ* obj) {
	glUseProgram(frameProg);

	vec2i frameSize = scale_and_offset_to_screen(obj->scale_percent, obj->scale_px);
	vec2i borderSize = (vec2i){ obj->borderWidth * 2, obj->borderWidth * 2 };
	vec2i totalFrameSize = vector_add_2i(frameSize, borderSize);
	vec2i framePos = scale_and_offset_to_screen(obj->pos_percent, obj->pos_px);
	vec2i topLeft = (vec2i){framePos.x - obj->borderWidth, framePos.y - obj->borderWidth};
	vec2i bottomRight = vector_add_2i(topLeft, totalFrameSize);
	vec2f frameBorderComposition = (vec2f){
		(float)obj->borderWidth / (float)totalFrameSize.x,
		(float)obj->borderWidth / (float)totalFrameSize.y
	};

	vec3f borderColor = (vec3f){
		obj->border_color.x / 255.0f,
		obj->border_color.y / 255.0f,
		obj->border_color.z / 255.0f
	};

	vec3f backgroundColor = (vec3f){
		obj->background_color.x / 255.0f,
		obj->background_color.y / 255.0f,
		obj->background_color.z / 255.0f
	};

	GLuint fbcLoc, bcLoc, btLoc, cLoc, tLoc, ssLoc, mpLoc;

	mpLoc = glGetUniformLocation(frameProg, "mousePos");
	fbcLoc = glGetUniformLocation(frameProg, "frameBorderComposition");
	bcLoc = glGetUniformLocation(frameProg, "borderColor");
	btLoc = glGetUniformLocation(frameProg, "borderTransparency");
	cLoc = glGetUniformLocation(frameProg, "color");
	tLoc = glGetUniformLocation(frameProg, "transparency");
	ssLoc = glGetUniformLocation(frameProg, "screenSize");
	
	glUniform2i(mpLoc, mousePos.x, mousePos.y);
	glUniform2i(ssLoc, screenSize.x, screenSize.y);
	glUniform2f(fbcLoc, frameBorderComposition.x, frameBorderComposition.y);
	glUniform3f(bcLoc, borderColor.x, borderColor.y, borderColor.z);
	glUniform1f(btLoc, obj->borderTransparancy);
	glUniform3f(cLoc, backgroundColor.x, backgroundColor.y, backgroundColor.z);
	glUniform1f(tLoc, obj->transparency);

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

	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_QUADS, 0, 4);
}

void destroy_PT_GUI_OBJ(void* obj) {
	PT_GUI_OBJ* guiObj = (PT_GUI_OBJ*)obj; // obj should be a PT_GUI_OBJ
	free(guiObj);
}

Instance* new_frame() {
	Instance* instance = new_instance();
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)calloc(1, sizeof(PT_GUI_OBJ));
	instance->subInstance = obj;
	instance->destroySubInstance = (void*)destroy_PT_GUI_OBJ;
	obj->instance = (void*)instance;
	
	return instance;
}