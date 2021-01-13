#ifndef PEACH_TEA_H
#define PEACH_TEA_H

#pragma comment(lib, "PeachTea")

#include "sort.h"
#include "tweenUtil.h"

#include "WinUtil.h"
#include "errorUtil.h"
#include "stringUtil.h"
#include "expandableArray.h"
#include "fontHandler.h"
#include "peachTime.h"

#include "glExtensions.h"
#include "shaders.h"
#include "glUniformUtil.h"
#include "PeachTeaShaders.h"

#include "matrixMath.h"
#include "vectorMath.h"

#include "fileUtil.h"
#include "imageLoader.h"

#include "mouse.h"
#include "keyboard.h"
#include "screenSize.h"

#include "Colors.h"
#include "glText.h"
#include "ScreenDimension.h"

#include "Instance.h"
#include "ScreenUI.h"
#include "guiObj.h"
#include "textLabel.h"
#include "imageLabel.h"
#include "scrollFrame.h"
#include "renderFrame.h"

#define PERFORMANCE_MODE 0

int PT_RUN(void(*updateCallback)(float), void(*renderCallback)(void));

void PT_CREATE_MAIN_WND(vec2i size, const char* title);
HWND PT_GET_MAIN_HWND();

PT_COLOR accentColor;

typedef enum {
	PT_PASTE,
	PT_COPY,
	PT_CUT,
	PT_SELECTALL
} PT_COMMAND;

PT_BINDABLE_EVENT eOnCommand;

#endif