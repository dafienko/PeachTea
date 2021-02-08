#include "PeachTeaShaders.h"
#include "glExtensions.h"
#include "assetLoader.h"

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

	float quadCorners[] = {
		0, 0,
		0, 1,
		1, 1,
		1, 0
	};

	glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners), quadCorners, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);


	GLuint qvs = create_vertex_shader("shaders\\core\\basicQuad.vs");
	GLuint tvs = create_vertex_shader("shaders\\core\\textQuad.vs");

	GLuint tqfs = create_fragment_shader("shaders\\core\\textQuad.fs");
	GLuint ifs = create_fragment_shader("shaders\\core\\imageQuad.fs");
	GLuint uifs = create_fragment_shader("shaders\\core\\UIframe.fs");
	GLuint bfs = create_fragment_shader("shaders\\core\\blur.fs");
	GLuint tfs = create_fragment_shader("shaders\\core\\tex.fs");
	GLuint rfs = create_fragment_shader("shaders\\core\\rectangle.fs");
	GLuint tsgfs = create_fragment_shader("shaders\\core\\textSpritemapGenerator.fs");

	GLuint quadShaders[] = { tvs, tqfs };
	PTS_text = create_program(quadShaders, 2);

	quadShaders[0] = qvs;
	quadShaders[1] = ifs;
	PTS_img = create_program(quadShaders, 2);

	quadShaders[1] = uifs;
	PTS_guiObj = create_program(quadShaders, 2);

	quadShaders[1] = bfs;
	PTS_blur = create_program(quadShaders, 2);

	quadShaders[1] = tfs;
	PTS_tex = create_program(quadShaders, 2);

	quadShaders[1] = rfs;
	PTS_rect = create_program(quadShaders, 2);

	quadShaders[1] = tsgfs;
	PTS_textSpritemapGenerator = create_program(quadShaders, 2);
}