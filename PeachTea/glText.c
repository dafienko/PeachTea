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

const int TEXT_BUFFER_SIZE = 100;
int renderIndex = 0;

GLuint* vao = NULL;
GLuint* vbos = NULL;

FT_Library ftLib;

void initFT() {
	FT_Error e = FT_Init_FreeType(&ftLib);
	if (e) {
		fatal_error(L"Failed to initialize freetype");
	}

	GLuint* vao = calloc(1, sizeof(GLuint));
	GLuint* vbos = calloc(4, sizeof(GLuint));
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	glGenBuffers(3, vbos);
}

FT_Face face = { 0 };
char_set create_char_set(const char* filename, const int textSize) {
	char_set cs = { 0 };
	cs.ssFactor = 4;
	cs.advance = calloc(128, sizeof(float));
	cs.size = calloc(128, sizeof(vec2i));
	cs.bearing = calloc(128, sizeof(vec2i));

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

	e = FT_Set_Char_Size(
		face,
		0,
		textSize * 64 * cs.ssFactor,
		screenSize.x,
		screenSize.y
	);
	if (e) {
		fatal_error(L"Failed to set char size");
	}

	e = FT_Set_Pixel_Sizes(
		face, 
		0, 
		textSize * cs.ssFactor
	);
	if (e) {
		fatal_error(L"Failed to set pixel size");
	}

	// load individual glyph textures into opengl
	GLuint* glyphTextures = calloc(128, sizeof(GLuint));
	int maxCharHeight = 0;
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

		GLuint glyphTex = 0;
		glGenTextures(1, &glyphTex);
		glBindTexture(GL_TEXTURE_2D, glyphTex);
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

		*(glyphTextures + i) = glyphTex;

		*(cs.advance + i) = face->glyph->advance.x / cs.ssFactor; // advance is in 64ths of a pixel
		*(cs.size + i) = (vec2i){face->glyph->bitmap.width / cs.ssFactor, face->glyph->bitmap.rows / cs.ssFactor };
		*(cs.bearing + i) = (vec2i){ face->glyph->bitmap_left / cs.ssFactor, face->glyph->bitmap_top / cs.ssFactor };

		if ((char)i == 'M') {
			cs.charSize = *(cs.size + i);
		}

		maxCharHeight = max(maxCharHeight, face->glyph->bitmap.rows / cs.ssFactor);
 	}
	cs.maxCharHeight = maxCharHeight;


	// create fbo w/ texture to hold all the glyphs in one spritesheet
	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	vec2i spriteSheetSize = (vec2i){ cs.charSize.x * 12 * cs.ssFactor, maxCharHeight * 12 * cs.ssFactor };
	GLuint spritesheet = 0;
	glGenTextures(1, &spritesheet);
	glBindTexture(GL_TEXTURE_2D, spritesheet);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		spriteSheetSize.x,
		spriteSheetSize.y,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		0
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, spritesheet, 0);

	// render all the individual glyph textures to one big spritesheet
	glViewport(0, 0, spriteSheetSize.x, spriteSheetSize.y);
	for (int i = 0; i < 128; i++) {
		glUseProgram(PTS_textSpritemapGenerator);

		GLuint glyphTex = *(glyphTextures + i);

		int x = i % 12;
		int y = i / 12;

		vec2i thisCharSize = *(cs.size + i);
		vec2i topLeft = (vec2i){ x * cs.charSize.x * cs.ssFactor, y * maxCharHeight * cs.ssFactor };
		vec2i bottomRight = (vec2i){ topLeft.x + thisCharSize.x * cs.ssFactor, topLeft.y + maxCharHeight * cs.ssFactor };
		
		uniform_vec2i(glGetUniformLocation(PTS_textSpritemapGenerator, "screenSize"), spriteSheetSize);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glyphTex);

		default_quad_corners();
		set_quad_positions(topLeft, bottomRight);

		glDrawArrays(GL_QUADS, 0, 4);
	}

	// delete all of the individual glyph textures
	glBindTexture(GL_TEXTURE_2D, 0); 
	glDeleteTextures(128, glyphTextures); 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0); // detach tex from framebuffer
	glDeleteFramebuffers(1, &fbo); // delete fbo, only the texture was needed
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind framebuffer

	cs.texture = spritesheet;

	free(glyphTextures);

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

	int renderIndex = 0;
	vec2f* vertexBuffer = calloc(4 * len, sizeof(vec2f));
	vec2f* posBuffer = calloc(4 * len, sizeof(vec2f));

	vec2i charSize = cs->charSize;

	float spritesheetWidth = 12.0f * cs->charSize.x * cs->ssFactor;
	float spritesheetHeight = 12.0f * cs->maxCharHeight * cs->ssFactor;
	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (int)floor((double)(penX - baseline_x) / (double)TAB_WIDTH) + 1;
			penX = baseline_x + nextTabIndex * TAB_WIDTH;
		}
		else if (c != '\n') {
			vec2i bearing = *(cs->bearing + c);
			vec2i thisCharSize = *(cs->size + c);

			vec2f topLeft = (vec2f){ penX + bearing.x, penY - bearing.y };
			vec2f bottomRight = vector_add_2f(topLeft, (vec2f){thisCharSize.x + 1, thisCharSize.y});
			
			*(vertexBuffer + renderIndex * 4 + 0) = (vec2f){ bottomRight.x, topLeft.y };
			*(vertexBuffer + renderIndex * 4 + 1) = (vec2f){ topLeft.x, topLeft.y };
			*(vertexBuffer + renderIndex * 4 + 2) = (vec2f){ topLeft.x, bottomRight.y };
			*(vertexBuffer + renderIndex * 4 + 3) = (vec2f){ bottomRight.x, bottomRight.y };

			int x = c % 12;
			int y = c / 12;

			topLeft = (vec2f){
				(cs->charSize.x * x * cs->ssFactor) / spritesheetWidth,
				1 - (y * cs->maxCharHeight * cs->ssFactor) / spritesheetHeight
			};
			bottomRight = (vec2f){
				(cs->charSize.x * (x + 1) * cs->ssFactor) / spritesheetWidth,
				1 - ((y + 1) * cs->maxCharHeight * cs->ssFactor) / spritesheetHeight
			};

			*(posBuffer + renderIndex * 4 + 0) = (vec2f){ bottomRight.x, topLeft.y };
			*(posBuffer + renderIndex * 4 + 1) = (vec2f){ topLeft.x, topLeft.y };
			*(posBuffer + renderIndex * 4 + 2) = (vec2f){ topLeft.x, bottomRight.y };
			*(posBuffer + renderIndex * 4 + 3) = (vec2f){ bottomRight.x, bottomRight.y };


			int advance = *(cs->advance + c);
			penX += advance >> 6;

			renderIndex++;
		}
	}

	glUseProgram(PTS_text);

	glBindBuffer(GL_ARRAY_BUFFER, *qVBO);
	glBufferData(GL_ARRAY_BUFFER, renderIndex * 4 * sizeof(vec2f), vertexBuffer, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, *(qVBO + 1));
	glBufferData(GL_ARRAY_BUFFER, renderIndex * 4 * sizeof(vec2f), posBuffer, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cs->texture);

	int ssLoc = glGetUniformLocation(PTS_text, "screenSize");
	int cLoc = glGetUniformLocation(PTS_text, "color");
	int tLoc = glGetUniformLocation(PTS_text, "transparency");
	int csLoc = glGetUniformLocation(PTS_text, "charSize");

	glUniform2i(ssLoc, viewportSize.x, viewportSize.y);
	uniform_PT_COLOR(cLoc, textColor);
	glUniform1f(tLoc, textTransparency);
	uniform_vec2i(csLoc, charSize);

	glDrawArrays(GL_QUADS, 0, 4 * renderIndex);

	free(vertexBuffer);
	free(posBuffer);
}

void PT_text_cleanup() {
	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(3, vbos);

	free(vao);
	free(vbos);
}

void free_char_set(char_set* cs) {
	glDeleteTextures(1, &cs->texture);

	free(cs->bearing);
	free(cs->advance);
	free(cs->size);
}


