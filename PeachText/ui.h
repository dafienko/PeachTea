#ifndef UI_H
#define UI_H

#include "PeachTea.h"

#define STATUS_BAR_HEIGHT 20
#define SIDE_BAR_WIDTH 40
#define SIDE_BAR_PADDING 7
#define SIDE_MENU_WIDTH 320

float sideFrameTransparency;

void update_rendertree();
PT_SCROLLFRAME* create_editor_scrollframe();

void collapse_sidebar();

typedef struct {
	PT_COLOR backgroundColor;
	PT_COLOR textColor;
	PT_COLOR accentColor;
	PT_COLOR editColor;
	PT_COLOR sidebarColor;
	PT_COLOR borderColor;
	PT_COLOR selectedLineColor;
	PT_COLOR cursorColor;
	int id;
} EDITOR_COLOR_THEME;

EDITOR_COLOR_THEME colorTheme;
EDITOR_COLOR_THEME* darkTheme;
EDITOR_COLOR_THEME* lightTheme;

Instance* screenUI;

PT_GUI_OBJ* backgroundObj;
PT_TEXTLABEL* statusBarLabel;
PT_GUI_OBJ* sideBarObj;
PT_TEXTLABEL* sidebarHeader;
PT_IMAGELABEL* menuButton;
PT_IMAGELABEL* saveButton;
PT_IMAGELABEL* saveAsButton;
PT_IMAGELABEL* newFileButton;
PT_IMAGELABEL* wordWrapButton;
PT_IMAGELABEL* themeButton;

void realize_color_theme(EDITOR_COLOR_THEME theme);
void init_ui();

#endif