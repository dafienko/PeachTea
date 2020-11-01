#ifndef PT_MOUSE_H
#define PT_MOUSE_H

#include "vectorMath.h"
#include "BindableEvent.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <hidusage.h>

vec2i mousePos;

PT_BINDABLE_EVENT e_mouseMove;
PT_BINDABLE_EVENT e_mouseDown;
PT_BINDABLE_EVENT e_mouseUp;

void mouse_init(HWND hMainWnd);

void mouse_input(RAWINPUT* raw);

void mouse_button1_down();

void mouse_button2_up();

#endif