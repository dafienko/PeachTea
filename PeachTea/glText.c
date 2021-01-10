#define _CRT_SECURE_NO_WARNINGS
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma comment(lib, "lib/freetype.lib")

#include <math.h>
#include "errorUtil.h"
#include "glExtensions.h"
#include "glText.h"
#include "ScreenUI.h"
#include "shaders.h"
#include "screenSize.h"
#include "glUniformUtil.h"
#include "PeachTeaShaders.h"

FT_Library ftLib;

void initFT() {
	FT_Error e = FT_Init_FreeType(&ftLib);
	if (e) {
		fatal_error(L"Failed to initialize freetype");
	}
}

void draw_quad(vec2i viewportSize, vec2i topLeft, vec2i bottomRight, GLuint tex, PT_COLOR textColor, float textTransparency) {
	glUseProgram(PTS_text);

	int ssLoc = glGetUniformLocation(PTS_text, "screenSize");
	int cLoc = glGetUniformLocation(PTS_text, "color");
	int tLoc = glGetUniformLocation(PTS_text, "transparency");
	int csLoc = glGetUniformLocation(PTS_text, "charSize");

	glUniform2i(ssLoc, viewportSize.x, viewportSize.y);
	uniform_PT_COLOR(cLoc, textColor);
	glUniform1f(tLoc, textTransparency);
	uniform_vec2i(csLoc, vector_sub_2i(bottomRight, topLeft));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	set_quad_positions(topLeft, bottomRight);
	default_quad_corners();

	glDrawArrays(GL_QUADS, 0, 4);
}

FT_Face face;
char_set create_char_set(const char* filename, const int textSize) {
	FILE* f = fopen(filename, "rb");
	int ee = errno;
	if (f == NULL) {
		fatal_windows_error(ee, L"LOL file failed to open");
	}
	fclose(f);

	FT_Error e = FT_New_Face(ftLib, filename, 0, &face);
	if (e) {
		fatal_error(L"Failed to load font");
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Set_Char_Size(
		face,
		0,
		textSize * 64,
		screenSize.x,
		screenSize.y
	);

	FT_Set_Pixel_Sizes(
		face, 
		0, 
		textSize
	);

	char_set cs = { 0 };
	cs.advance = calloc(128, sizeof(float));
	cs.size = calloc(128, sizeof(vec2i));
	cs.bearing = calloc(128, sizeof(vec2i));
	cs.textures = calloc(128, sizeof(GLuint));
	
	for (int i = 0; i < 128; i++) {
		int glyphIndex = FT_Get_Char_Index(face, (char)i);
		e = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
		if (!e) {
			e = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		}
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
		*(cs.advance + i) = face->glyph->advance.x; // advance is in 64ths of a pixel
		*(cs.size + i) = (vec2i){face->glyph->bitmap.width, face->glyph->bitmap.rows};
		*(cs.bearing + i) = (vec2i){ face->glyph->bitmap_left, face->glyph->bitmap_top };
 	}

	return cs;
}

const int TAB_WIDTH = 60;

int get_text_width(char_set* cs, const char* str, int len) {
	int penX = 0;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (int)floor((double)penX / (double)TAB_WIDTH) + 1;
			penX = nextTabIndex * TAB_WIDTH;
		}
		else if (c != '\n') {
			int advance = *(cs->advance + c);
			penX += advance >> 6;
		}
	}

	return penX;
}

void render_text(vec2i viewportSize, char_set* cs, PT_COLOR textColor, float textTransparency, const char* str, int len, int baseline_x, int baseline_y) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int penX = baseline_x;
	int penY = baseline_y;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (int)floor((double)(penX - baseline_x) / (double)TAB_WIDTH) + 1;
			penX = baseline_x + nextTabIndex * TAB_WIDTH;
		}
		else if (c != '\n') {
			vec2i bearing = *(cs->bearing + c);
			vec2i size = *(cs->size + c);

			vec2i topLeft = (vec2i){ penX + bearing.x, penY - bearing.y };
			vec2i bottomRight = vector_add_2i(topLeft, size);

			draw_quad(viewportSize, topLeft, bottomRight, *(cs->textures + c), textColor, textTransparency);

			int advance = *(cs->advance + c);
			penX += advance >> 6;
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


