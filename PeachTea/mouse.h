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

PT_BINDABLE_EVENT e_mouse1Down;
PT_BINDABLE_EVENT e_mouse1Up;

PT_BINDABLE_EVENT e_wheelUp;
PT_BINDABLE_EVENT e_wheelDown;

void mouse_init(HWND hMainWnd);

void mouse_input(RAWINPUT* raw);

#endif