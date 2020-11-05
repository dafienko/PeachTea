#ifndef PEACH_TEA_H
#define PEACH_TEA_H

#pragma comment(lib, "PeachTea")

#include "WinUtil.h"
#include "errorUtil.h"

#include "glExtensions.h"
#include "shaders.h"

#include "matrixMath.h"
#include "vectorMath.h"

#include "fileUtil.h"
#include "imageLoader.h"

#include "mouse.h"
#include "screenSize.h"

#include "Colors.h"
#include "glText.h"
#include "guiUtil.h"
#include "ScreenDimension.h"

#include "Instance.h"
#include "ScreenUI.h"
#include "guiObj.h"
#include "textLabel.h"
#include "imageLabel.h"

int PT_RUN(void(*renderCallback)(void));

void PT_CREATE_MAIN_WND(vec2i size, const char* title);
void PT_GET_MAIN_HWND();

#endif