#ifndef TEXT_EDITOR_HANDLER_H
#define TEXT_EDITOR_HANDLER_H

#include "PeachTea.h"
#include "textCursor.h"

typedef struct {
	char* str;
	int numChars;
	int numCharSpace;
	PT_EXPANDABLE_ARRAY flags;
} TEXT_LINE;

typedef struct {
	struct TEXT_EDITOR* editor;

	PT_GUI_OBJ* main;
	PT_TEXTLABEL* header;
	PT_TEXTLABEL* desc;

	PT_EXPANDABLE_ARRAY selectTweens;
	PT_EXPANDABLE_ARRAY deselectTweens;
	PT_EXPANDABLE_ARRAY onhoverTweens;
} EDITOR_LIST_ELEMENT;

typedef struct {
	int textHeight, charWidth, linePadding;
	char_set* charSet;

	PT_COLOR textColor;
	PT_COLOR editColor;
	float editFadeTime;

	PT_SCROLLFRAME* scrollFrame;
	PT_RENDERFRAME* renderFrame;
	PT_RENDERFRAME* sideRenderFrame;

	PT_COLOR insertionColor;
	float insertionFadeTime;

	PT_EXPANDABLE_ARRAY* textLines;
	TEXT_CURSOR textCursor;

	char* path;
	char* filename;
	char* extension;

	EDITOR_LIST_ELEMENT listElement;
	
	int saved;
} TEXT_EDITOR;

TEXT_METADATA_FLAG create_text_metadata_flag(float t);

TEXT_LINE TEXT_LINE_new(const char* str, int len);
void TEXT_LINE_destroy(TEXT_LINE* line);

char get_last_char(TEXT_LINE line);

TEXT_EDITOR* TEXT_EDITOR_new(Instance* scrollframe, PT_RENDERFRAME* renderFrame, PT_RENDERFRAME* sideRenderFrame, PT_SCROLLFRAME* listContainer);
TEXT_EDITOR* TEXT_EDITOR_from_file(Instance* scrollframe, PT_RENDERFRAME* renderFrame, PT_RENDERFRAME* sideRenderFrame, PT_SCROLLFRAME* listContainer, const char* filename);
void TEXT_EDITOR_save(TEXT_EDITOR* textEditor);

void TEXT_EDITOR_update_list_element(TEXT_EDITOR* editor);
void TEXT_EDITOR_select(TEXT_EDITOR* textEditor);
int TEXT_EDITOR_get_margin(TEXT_EDITOR* editor);
int TEXT_EDITOR_get_wrapX(TEXT_EDITOR* editor);

vec2i TEXT_EDITOR_screenPos_to_cursorPos(vec2i screenPos);
void move_text_pos_in_view(vec2i textPosition);

void TEXT_EDITOR_update(TEXT_EDITOR* editor, float dt);

TEXT_EDITOR* get_current_text_editor();
int* get_charsTyped();

int wrapText;
int insertMode;

#endif