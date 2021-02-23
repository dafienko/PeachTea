#ifndef UI_H
#define UI_H

#include "PeachTea.h"

#define STATUS_BAR_HEIGHT 20
#define SIDE_BAR_WIDTH 40
#define SIDE_BAR_PADDING 7
#define SIDE_MENU_WIDTH 320

void update_rendertree();
PT_SCROLLFRAME* create_editor_scrollframe();

void collapse_sidebar();

#endif