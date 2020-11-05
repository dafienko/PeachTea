#include "PeachTeaShaders.h"
#include "glExtensions.h"
#include "assetLoader.h"

void PT_SHADERS_init() {
	GLuint qvs = create_vertex_shader("shaders\\basicQuad.vs");

	GLuint tfs = create_fragment_shader("shaders\\textQuad.fs");
	GLuint ifs = create_fragment_shader("shaders\\imageQuad.fs");
	GLuint uifs = create_fragment_shader("shaders\\UIframe.fs");
	
	GLuint quadShaders[] = { qvs, tfs };
	PTS_text = create_program(quadShaders, 2);

	quadShaders[1] = ifs;
	PTS_img = create_program(quadShaders, 2);

	quadShaders[1] = uifs;
	PTS_guiObj = create_program(quadShaders, 2);
}