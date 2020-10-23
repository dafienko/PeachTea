#define _CRT_SECURE_NO_WARNINGS

#include "renderer.h"
#include "PeachTea.h"
#include <stdlib.h>
#include "screenSize.h"

#include "uiContainer.h"

vec2i vpSize;
int initialized = 0;

char_set cs;

void renderer_resized() {
	glViewport(0, 0, screenSize.x, screenSize.y);
	vpSize = screenSize;
}

void renderer_init() {
	renderer_resized();

	initialized = 1;
}

void render(void(*renderCallback)(void)) {
	if (initialized) {
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT);

		renderCallback();

		glFlush();
	}
}

