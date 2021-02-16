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

const int TEXT_BUFFER_SIZE = 300;

GLuint* textvao = NULL;
GLuint* textvbos = NULL;

FT_Library ftLib = NULL;

void initFT() {
	FT_Error e = FT_Init_FreeType(&ftLib);
	if (e) {
		fatal_error(L"Failed to initialize freetype");
	}

	textvao = calloc(1, sizeof(GLuint));
	textvbos = calloc(4, sizeof(GLuint));
	glGenVertexArrays(1, textvao);
	glBindVertexArray(*textvao);

	glGenBuffers(3, textvbos);

	glBindBuffer(GL_ARRAY_BUFFER, *(textvbos + 0));
	glBufferData(GL_ARRAY_BUFFER, (4 * TEXT_BUFFER_SIZE * (int)sizeof(vec2f)), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, *(textvbos + 1));
	glBufferData(GL_ARRAY_BUFFER, 4 * TEXT_BUFFER_SIZE * (int)sizeof(vec2f), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void TEXT_METADATA_FLAG_insert(PT_EXPANDABLE_ARRAY* flags, TEXT_METADATA_FLAG flag) {
	int insertIndex = 0;
	int flagStrIndex = flag.index;
	for (int i = 0; i < flags->numElements; i++) {
		TEXT_METADATA_FLAG thisFlag = *(TEXT_METADATA_FLAG*)PT_EXPANDABLE_ARRAY_get(flags, i);
		int thisIndex = thisFlag.index;

		if (thisIndex > flagStrIndex) {
			break;
		} 
		else if (thisIndex == flagStrIndex) {
			fatal_error(L"Tried to insert a flag at an existing flag position (%i == %i)", thisIndex, flagStrIndex);
		}
		else {
			insertIndex++;
		}
	}

	PT_EXPANDABLE_ARRAY_insert(flags, insertIndex, &flag);
}

FT_Face face = { 0 };
char_set create_char_set(const char* filename, const int textSize) {
	char_set cs = { 0 };
	cs.ssFactor = 2; // super-sample factor
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

	int dpi = 100;
	e = FT_Set_Char_Size(
		face,
		0,
		textSize * 64 * cs.ssFactor,
		dpi,
		dpi
	);
	if (e) {
		fatal_error(L"Failed to set char size");
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

		*(cs.advance + i) = (float)face->glyph->advance.x / cs.ssFactor; // advance is in 64ths of a pixel
		*(cs.size + i) = (vec2i){face->glyph->bitmap.width / cs.ssFactor, face->glyph->bitmap.rows / cs.ssFactor };
		*(cs.bearing + i) = (vec2i){ face->glyph->bitmap_left / cs.ssFactor, face->glyph->bitmap_top / cs.ssFactor };

		vec2i thisSize = *(cs.size + i);
		cs.charSize = (vec2i){ max(cs.charSize.x, thisSize.x), max(cs.charSize.y, thisSize.y) };

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, spritesheet, 0);

	// render all the individual glyph textures to one big spritesheet
	glViewport(0, 0, spriteSheetSize.x, spriteSheetSize.y);
	for (int i = 0; i < 128; i++) {
		glUseProgram(PTS_textSpritemapGenerator);

		GLuint glyphTex = *(glyphTextures + i);

		int x = i % 12;
		int y = i / 12;

		vec2i thisCharSize = *(cs.size + i);
		vec2i topLeft = (vec2i){ x * cs.charSize.x * cs.ssFactor, y * cs.charSize.y * cs.ssFactor };
		vec2i bottomRight = (vec2i){ topLeft.x + thisCharSize.x * cs.ssFactor, topLeft.y + thisCharSize.y * cs.ssFactor };
		
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


vec2i get_text_rect(char_set* cs, const char* str, int len, int wrapX) {
	int tabWidth = cs->charSize.x * 5;
	int maxX = 0;
	int penY = 0;
	int penX = 0;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = penX / tabWidth + 1;
			penX = nextTabIndex * tabWidth;
		}
		else if (c != '\n') {
			int advance = (int)*(cs->advance + c);
			advance >>= 6;

			if (wrapX && (penX + advance > wrapX)) {
				penX = 0;
				penY += 1;
			}

			penX += advance;
		}
		else {
			int advance = (int)*(cs->advance + c);
			advance = advance >> 6;
		}

		maxX = max(penX, maxX);
	}

	return (vec2i){maxX, penY};
}

vec2i get_text_offset(char_set* cs, const char* str, int len, int wrapX) {
	int tabWidth = cs->charSize.x * 5;
	int penY = 0;
	int penX = 0;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = penX / tabWidth + 1;
			penX = nextTabIndex * tabWidth;
		}
		else if (c != '\n') {
			int advance = (int)*(cs->advance + c);
			advance >>= 6;
			
			if (wrapX && (penX + advance > wrapX)) {
				penX = 0;
				penY += 1;
			}

			penX += advance																																											;
		}
		else {
			penX = 0;
			penY++;
		}
	}

	return (vec2i) { penX, penY };
}

int get_char_position(char_set* cs, const char* str, int len, int x) {
	int penX = 0;
	int cIndex = 0;
	int distanceToIndex = -1;
	int tabWidth = cs->charSize.x * 5;

	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c != '\n') {
			int d = abs(penX - x);
			if (distanceToIndex == -1 || d < distanceToIndex) {
				distanceToIndex = d;
				cIndex = i;
			}
		}

		if (c == '\t') {
			int nextTabIndex = penX / tabWidth + 1;
			penX = nextTabIndex * tabWidth;
		}
		else if (c != '\n') {
			int advance = (int)*(cs->advance + c);
			penX += advance >> 6;
		}

		if (c != '\n') {
			int d = abs(penX - x);
			if (distanceToIndex == -1 || d < distanceToIndex) {
				distanceToIndex = d;
				cIndex = i + 1;
			}
		}
	}


	return cIndex;
}

void render_text_section(int sectionStart, int sectionEnd, vec2f* vertexBuffer, vec2f* posBuffer) {
	vertexBuffer += sectionStart * 4;
	posBuffer += sectionStart * 4;

	int totalSectionSize = sectionEnd - sectionStart;
	for (int i = 0; 1; i++) {
		int chunkRenderIndexStart = TEXT_BUFFER_SIZE * i; // 0, n, 2n, 3n, ...
		if (chunkRenderIndexStart >= totalSectionSize) {
			break;
		}

		int chunkRenderIndexEnd = min(TEXT_BUFFER_SIZE * (i + 1) - 1, totalSectionSize - 1); // inclusive bound   n-1, 2n-1, 3n-1, 4n-1, ...
		int chunkSize = (chunkRenderIndexEnd - chunkRenderIndexStart) + 1; // add one, from 1-0=1 but from 0 to 1 is two indices

		glBindBuffer(GL_ARRAY_BUFFER, *(textvbos + 0));
		glBufferSubData(GL_ARRAY_BUFFER, 0, chunkSize * 4 * sizeof(vec2f), vertexBuffer + chunkRenderIndexStart * 4);

		glBindBuffer(GL_ARRAY_BUFFER, *(textvbos + 1));
		glBufferSubData(GL_ARRAY_BUFFER, 0, chunkSize * 4 * sizeof(vec2f), posBuffer + chunkRenderIndexStart * 4);

		glDrawArrays(GL_QUADS, 0, 4 * chunkSize);
	}
}

void render_text(vec2i viewportSize, char_set* cs, PT_COLOR textColor, float textTransparency, const char* str, int len, int baseline_x, int baseline_y, int wrapX, int lineWidth, PT_EXPANDABLE_ARRAY* metadataFlags) {
	if (!lineWidth) {
		lineWidth = cs->charSize.y;
	}

	TEXT_METADATA_FLAG currentFlag = { 0 };
	currentFlag.color = textColor;
	TEXT_METADATA_FLAG nextFlag = { 0 };
	nextFlag.index = len;
	if (metadataFlags) {
		currentFlag = *(TEXT_METADATA_FLAG*)PT_EXPANDABLE_ARRAY_get(metadataFlags, 0);

		if (metadataFlags->numElements > 1) {
			nextFlag = *(TEXT_METADATA_FLAG*)PT_EXPANDABLE_ARRAY_get(metadataFlags, 1);
		}
	}

	int tabWidth = cs->charSize.x * 5;
	int penX = baseline_x;
	int penY = baseline_y;

	int renderIndex = 0;
	vec2f* vertexBuffer = calloc(4 * len, sizeof(vec2f));
	vec2f* posBuffer = calloc(4 * len, sizeof(vec2f));

	vec2i charSize = cs->charSize;

	// spritesheet is 12x12 chars wide
	float spritesheetWidth = 12.0f * cs->charSize.x * cs->ssFactor;
	float spritesheetHeight = 12.0f * cs->maxCharHeight * cs->ssFactor;
	for (int i = 0; i < len; i++) {
		char c = *(str + i);

		if (c == '\t') {
			int nextTabIndex = (penX - baseline_x) / (tabWidth) + 1;
			penX = baseline_x + nextTabIndex * tabWidth;
		}
		else if (c != '\n') {
			int advance = (int)*(cs->advance + c);
			advance = advance >> 6;

			if (wrapX && (penX + advance > wrapX)) {
				penX = baseline_x;
				penY += lineWidth;
			}


			vec2i bearing = *(cs->bearing + c);
			vec2i thisCharSize = *(cs->size + c);

			vec2f topLeft = (vec2f){ (float)(penX + bearing.x), (float)(penY - bearing.y) };
			vec2f bottomRight = vector_add_2f(topLeft, (vec2f) { (float)(thisCharSize.x + 1), (float)thisCharSize.y });
			
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
				topLeft.x + (thisCharSize.x * cs->ssFactor) / spritesheetWidth,
				topLeft.y - (thisCharSize.y * cs->ssFactor) / spritesheetHeight
			};

			*(posBuffer + renderIndex * 4 + 0) = (vec2f){ bottomRight.x, topLeft.y };
			*(posBuffer + renderIndex * 4 + 1) = (vec2f){ topLeft.x, topLeft.y };
			*(posBuffer + renderIndex * 4 + 2) = (vec2f){ topLeft.x, bottomRight.y };
			*(posBuffer + renderIndex * 4 + 3) = (vec2f){ bottomRight.x, bottomRight.y };

			penX += advance;

			renderIndex++;
		}
	}


	glUseProgram(PTS_text);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int ssLoc = glGetUniformLocation(PTS_text, "screenSize");
	int cLoc = glGetUniformLocation(PTS_text, "textColor");
	int tLoc = glGetUniformLocation(PTS_text, "transparency");

	glUniform2i(ssLoc, viewportSize.x, viewportSize.y);
	glUniform1f(tLoc, textTransparency);

	glBindVertexArray(*textvao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cs->texture);

	int numFlags = 1;
	if (metadataFlags) {
		numFlags = metadataFlags->numElements;
	}



	///*
	if (numFlags > 1) {
		//printf("\n");
	}
	for (int i = 0; i < numFlags; i++) {
		if (metadataFlags) {
			currentFlag = *(TEXT_METADATA_FLAG*)PT_EXPANDABLE_ARRAY_get(metadataFlags, i);
			
			if (i + 1 < metadataFlags->numElements) {
				nextFlag = *(TEXT_METADATA_FLAG*)PT_EXPANDABLE_ARRAY_get(metadataFlags, i + 1);
			}
		}
		

		PT_COLOR color = PT_COLOR_fromHSV(0, 0, (float)(i + 1.0f) / (float)numFlags);
		color = currentFlag.color;
		uniform_PT_COLOR(cLoc, color);
		
		int startIndex = currentFlag.index;

		int nextIndex = nextFlag.index;
		if (i == numFlags - 1) {
			nextIndex = renderIndex;
		}

		if (numFlags > 1) {
			//printf("%i -> %i    (%.2f, %.2f, %.2f)\n", startIndex, nextFlag.index, currentFlag.color.r, currentFlag.color.g, currentFlag.color.b);
		}

		if (currentFlag.index < len) {
			render_text_section(startIndex, nextIndex, vertexBuffer, posBuffer);
		}
	}
	//*/

	free(vertexBuffer);
	free(posBuffer);
}

void PT_text_cleanup() {
	glDeleteVertexArrays(1, textvao);
	glDeleteBuffers(3, textvbos);

	free(textvao);
	free(textvbos);
}

void free_char_set(char_set* cs) {
	glDeleteTextures(1, &cs->texture);

	free(cs->bearing);
	free(cs->advance);
	free(cs->size);
}

int get_text_position_from_rel_position(char_set* cs, char* str, int len, vec2i relPos, int lineThickness, int wrapX) {
	int penX = 0;
	int penY = 0;
	int cIndex = 0;
	int distanceToIndex = -1;
	int tabWidth = cs->charSize.x * 5;

	for (int i = 0; i <= len; i++) {
		if (relPos.y >= penY && relPos.y < penY + lineThickness) {
			int dist = abs(relPos.x - penX);
			if (distanceToIndex == -1 || dist < distanceToIndex) {
				cIndex = i;
				distanceToIndex = dist;
			}
		}
		else if (relPos.y < penY) {
			break;
		}

		if (i < len) {
			char c = *(str + i);

			if (c == '\t') {
				int nextTabIndex = penX / tabWidth + 1;
				penX = nextTabIndex * tabWidth;
			}
			else if (c != '\n') {
				int advance = (int)*(cs->advance + c);
				advance = advance >> 6;

				if (wrapX && (penX + advance > wrapX)) {
					penX = 0;
					penY += lineThickness;
				}

				penX += advance;
			}
			else {
				penX = 0;
				penY += lineThickness;
			}
		}
	}

	return cIndex;
}
