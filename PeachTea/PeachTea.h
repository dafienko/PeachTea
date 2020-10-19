#ifndef PEACH_TEA_H
#define PEACH_TEA_H

#pragma comment(lib, "PeachTea")

#include "WinUtil.h"
#include "errorUtil.h"
#include "glExtensions.h"
#include "glText.h"
#include "shaders.h"
#include "fileUtil.h"
#include "matrixMath.h"
#include "vectorMath.h"
#include "guiUtil.h"
#include "mouse.h"
#include "screenSize.h"
#include "imageLoader.h"

int PT_RUN();

void PT_CREATE_MAIN_WND(vec2i size, const char* title);
void PT_GET_MAIN_HWND();

#endif