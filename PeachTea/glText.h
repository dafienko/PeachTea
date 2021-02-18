#ifndef GL_TEXT_H
#define GL_TEXT_H

#include "glExtensions.h"
#include "vectorMath.h"
#include "Colors.h"
#include "expandableArray.h"

typedef struct {
	GLuint texture;
	
	vec2i* size;
	vec2i* bearing;
	float* advance;
	int num_chars;

	vec2i charSize;
	int maxCharHeight;
	int textHeight;

	int ssFactor;
} char_set;

typedef struct {
	int index;
	PT_COLOR color;
	void* misc; // context-dependent, user can make each metadata misc pointer point to whatever they want
} TEXT_METADATA_FLAG;

void initFT();

char_set create_char_set(const char* font, const int fontSize);
void free_char_set(char_set* cs);

void render_text(vec2i viewportSize, char_set* cs, PT_COLOR textColor, float textTransparency, const char* str, int len, int baseline_x, int baseline_y, int wrapX, int lineWidth, PT_EXPANDABLE_ARRAY* metadataFlags);
vec2i get_text_rect(char_set* cs, const char* str, int len, int wrapX);
vec2i get_text_offset(char_set* cs, const char* str, int len, int wrapX);

int get_char_position(char_set* cs, const char* str, int len, int x);
int get_text_position_from_rel_position(char_set* cs, char* str, int len, vec2i relPos, int lineThickness, int wrapX);

void TEXT_METADATA_FLAG_insert(PT_EXPANDABLE_ARRAY* flags, TEXT_METADATA_FLAG flag);


#endif