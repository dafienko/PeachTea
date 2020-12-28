#include "frameTexture.h"
#include "PeachTeaShaders.h"
#include "errorUtil.h"
#include "screenSize.h"
#include "ScreenUI.h"
#include "glUniformUtil.h"
#include "expandableArray.h"

PT_FRAMETEXTURE PT_FRAMETEXTURE_new(int w, int h) {
	PT_FRAMETEXTURE frameTexture = { 0 };

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fatal_error(L"frametexture creation failed");
	}


	frameTexture.width = w;
	frameTexture.height = h;
	frameTexture.fbo = fbo;
	frameTexture.tex = tex;
	frameTexture.rbo = rbo;

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // switch back to default framebuffer once this framebuffer has been created

	return frameTexture;
}

PT_EXPANDABLE_ARRAY framebuffersBoundToResize;

void ft_on_resize(void* arg) {
	PT_FRAMETEXTURE** frametextures = (PT_FRAMETEXTURE**)framebuffersBoundToResize.data;
	for (int i = 0; i < framebuffersBoundToResize.numElements; i++) {
		PT_FRAMETEXTURE* frameTexture = *(frametextures + i);
		*frameTexture = PT_FRAMETEXTURE_resize(*frameTexture, screenSize.x, screenSize.y);
	}
}

PT_FRAMETEXTURE_bind_to_screensize(PT_FRAMETEXTURE* frameTexture) {
	if (framebuffersBoundToResize.data == NULL) { // init framebuffersBoundToResize
		framebuffersBoundToResize = PT_EXPANDABLE_ARRAY_new(5, sizeof(PT_FRAMETEXTURE*));

		PT_BINDABLE_EVENT_bind(&eOnResize, ft_on_resize);
	}

	PT_EXPANDABLE_ARRAY_add(&framebuffersBoundToResize, &frameTexture);
}

PT_FRAMETEXTURE PT_FRAMETEXTURE_resize(PT_FRAMETEXTURE frameTexture, int w, int h) {
	PT_FRAMETEXTURE_destroy(frameTexture);

	return PT_FRAMETEXTURE_new(w, h);
}

void PT_FRAMETEXTURE_destroy(PT_FRAMETEXTURE tex) {
	glBindFramebuffer(GL_FRAMEBUFFER, tex.fbo);

	// unbind fbo renderbuffer and texture
	glFramebufferTexture2D(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, GL_RGBA, 0, 0);
	glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);

	glDeleteRenderbuffers(1, &tex.rbo);
	glDeleteFramebuffers(1, &tex.fbo);
	glDeleteTextures(1, &tex.tex);
}


void PT_FRAMETEXTURE_bind(PT_FRAMETEXTURE tex) {
	glBindFramebuffer(GL_FRAMEBUFFER, tex.fbo);
	glViewport(0, 0, tex.width, tex.height);
}

void PT_FRAMETEXTURE_unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenSize.x, screenSize.y);
}


// texture effects
void PT_FRAMETEXTURE_blur(GLuint src, PT_FRAMETEXTURE dest, vec2f dir, int blurRadius, int textureFlipped) {
	int s = 1;
	int r = blurRadius;
	
	if (r > 50) {
		s = 3;
		r /= 3;
	}
	else if (r > 20) {
		s = 2;
		r /= 2;
	}
	else if (r > 5) {
		s = 2;
		r /= 2;
	}


	PT_FRAMETEXTURE_bind(dest);
	glUseProgram(PTS_blur);

	int ssLoc, brLoc, bdLoc, osLoc;

	ssLoc = glGetUniformLocation(PTS_blur, "screenSize");
	brLoc = glGetUniformLocation(PTS_blur, "blurRadius");
	bdLoc = glGetUniformLocation(PTS_blur, "blurDir");
	osLoc = glGetUniformLocation(PTS_blur, "offsetScale");

	uniform_vec2i(ssLoc, screenSize);
	uniform_vec2f(bdLoc, dir);
	glUniform1i(brLoc, r);
	glUniform1f(osLoc, s);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src);

	if (textureFlipped) {
		set_quad_corners((vec2f) { 0, 1 }, (vec2f) { 1, 0 });
	}
	else {
		default_quad_corners();
	}
	set_quad_positions((vec2i) { 0 }, screenSize);

	glDrawArrays(GL_QUADS, 0, 4);
}

void PT_FRAMETEXTURE_copy_to_framebuffer(PT_FRAMETEXTURE frameTexture, GLuint framebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, frameTexture.width, frameTexture.height);

	glUseProgram(PTS_tex);

	uniform_vec2i(glGetUniformLocation(PTS_tex, "screenSize"), screenSize);

	set_quad_positions((vec2i) { 0 }, screenSize);
	set_quad_corners((vec2f) { 0, 1 }, (vec2f) { 1, 0 });

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTexture.tex);

	glDrawArrays(GL_QUADS, 0, 4);
}