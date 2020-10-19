#define _CRT_SECURE_NO_WARNINGS

#include "renderer.h"
#include "PeachTea.h"
#include <stdlib.h>
#include "screenSize.h"

vec2i vpSize;
int initialized = 0;
Instance* testFrameInstance;
PT_GUI_OBJ* testFrame;

char_set cs;

void renderer_resized() {
	glViewport(0, 0, screenSize.x, screenSize.y);
	vpSize = screenSize;
}

void renderer_init() {
	renderer_resized();

	testFrameInstance = new_frame();
	testFrame = (PT_GUI_OBJ*)testFrameInstance->subInstance;
	testFrame->transparency = .5;
	testFrame->borderTransparancy = .5;
	testFrame->background_color = (vec3i){ 0, 0, 0 };
	testFrame->border_color = (vec3i){ 255, 0, 0 };
	testFrame->borderWidth = 1;
	testFrame->scale_percent = (vec2f){ .33f, .33f };
	//testFrame->scale_px = (vec2i){ 300, 200 };
	testFrame->pos_percent = (vec2f){ .33, .33 };
	//testFrame->pos_px = (vec2i){ 100, 100 };

	cs = create_char_set("assets\\fonts\\comic.ttf", 24);

	initialized = 1;
}

void render() {
	if (initialized) {
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT);

		char* str = calloc(400, sizeof(char));
		for (int y = 1; y < 20; y++) {
			sprintf(str, "%i: \ta;l\t\tksjf", y);
			render_text(&cs, str, 0, y * (24 + 2));
		}
		free(str);

		render_default_obj(testFrame);

		glFlush();
	}
}

