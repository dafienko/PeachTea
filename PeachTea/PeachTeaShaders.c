#include "PeachTeaShaders.h"
#include "glExtensions.h"
#include "assetLoader.h"

float DEFAULT_QUAD_CORNERS[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
};

void PT_SHADERS_init() {
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

	glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glBufferData(GL_ARRAY_BUFFER, sizeof(DEFAULT_QUAD_CORNERS), DEFAULT_QUAD_CORNERS, GL_STATIC_DRAW);

	GLuint qvs = create_vertex_shader("shaders\\basicQuad.vs");

	GLuint tqfs = create_fragment_shader("shaders\\textQuad.fs");
	GLuint ifs = create_fragment_shader("shaders\\imageQuad.fs");
	GLuint uifs = create_fragment_shader("shaders\\UIframe.fs");
	GLuint bfs = create_fragment_shader("shaders\\blur.fs");
	GLuint tfs = create_fragment_shader("shaders\\tex.fs");

	GLuint quadShaders[] = { qvs, tqfs };
	PTS_text = create_program(quadShaders, 2);

	quadShaders[1] = ifs;
	PTS_img = create_program(quadShaders, 2);

	quadShaders[1] = uifs;
	PTS_guiObj = create_program(quadShaders, 2);

	quadShaders[1] = bfs;
	PTS_blur = create_program(quadShaders, 2);

	quadShaders[1] = tfs;
	PTS_tex = create_program(quadShaders, 2);
}