#define _CRT_SECURE_NO_WARNINGS
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma comment(lib, "lib/freetype.lib")

#include <math.h>
#include "errorUtil.h"
#include "glExtensions.h"
#include "glText.h"
#include "guiUtil.h"
#include "shaders.h"
#include "screenSize.h"

GLuint qProg;

FT_Library ftLib;
FT_Face face;

void initFT() {
	FT_Error e = FT_Init_FreeType(&ftLib);
	if (e) {
		fatal_error(L"Failed to initialize freetype");
	}

	GLuint qvs = create_vertex_shader("shaders\\basicQuad.vs");
	GLuint qfs = create_fragment_shader("shaders\\texturedQuad.fs");
	GLuint quadShaders[] = { qvs, qfs };
	qProg = create_program(quadShaders, 2);
}

void draw_quad(vec2i topLeft, vec2i bottomRight, GLuint tex) {
	glUseProgram(qProg);

	int ssLoc = glGetUniformLocation(qProg, "screenSize");
	glUniform2i(ssLoc, screenSize.x, screenSize.y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

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

char_set create_char_set(const char* filename, const int textSize) {
	FILE* f = fopen(filename, "rb");
	int ee = errno;
	if (f == NULL) {
		fatal_windows_error(ee, L"LOL file failed to open");
	}
	fclose(f);

	FT_Error e = FT_New_Face(ftLib, filename, 0, &face);
	if (e) {
		
		fatal_error(L"Failed to load comic.ttf");
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Set_Pixel_Sizes(face, 0, textSize);

	char_set cs = { 0 };
	cs.advance = calloc(128, sizeof(int));
	cs.size = calloc(128, sizeof(vec2i));
	cs.bearing = calloc(128, sizeof(vec2i));
	cs.textures = calloc(128, sizeof(GLuint));

	for (int i = 0; i < 128; i++) {
		e = FT_Load_Char(face, (char)i, FT_LOAD_RENDER);
		if (e)
		{
			fatal_error(L"Failed to load \'%i\' character", i);
		}

		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		*(cs.textures + i) = texture;
		*(cs.advance + i) = (int)(face->glyph->advance.x / 64.0); // advance is in 64ths of a pixel
		*(cs.size + i) = (vec2i){face->glyph->bitmap.width, face->glyph->bitmap.rows};
		*(cs.bearing + i) = (vec2i){ face->glyph->bitmap_left, face->glyph->bitmap_top };	
 	}

	return cs;
}

const int TAB_WIDTH = 60;

int get_text_width(char_set* cs, const char* str) {
	int len = strlen(str);
	int xOff = 0;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (int)floor((double)xOff / (double)TAB_WIDTH) + 1;
			xOff = nextTabIndex * TAB_WIDTH;
		}
		else if (c != '\n') {
			int advance = *(cs->advance + c);
			xOff += advance;
		}
	}

	return xOff;
}

void render_text(char_set* cs, const char* str, int baseline_x, int baseline_y) {
	int xOff = 0;
	int len = strlen(str);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (int)floor((double)xOff / (double)TAB_WIDTH) + 1;
			xOff = nextTabIndex * TAB_WIDTH;
		}
		else if (c != '\n') {
			vec2i bearing = *(cs->bearing + c);
			vec2i size = *(cs->size + c);

			vec2i topLeft = (vec2i){ baseline_x + xOff, baseline_y - bearing.y };
			vec2i bottomRight = vector_add_2i(topLeft, size);

			draw_quad(topLeft, bottomRight, *(cs->textures + c));

			int advance = *(cs->advance + c);
			xOff += advance;
		}
	}
}

void free_char_set(char_set* cs) {
	for (int i = 0; i < 128; i++) {
		glDeleteTextures(1, *(cs->textures + i));
	}

	free(cs->bearing);
	free(cs->advance);
	free(cs->size);
}


