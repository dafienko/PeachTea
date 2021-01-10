#include "mouse.h"
#include "screenSize.h"

#include <stdio.h>

RAWINPUTDEVICE Rid[1] = { 0 };

void mouse_init(HWND hWnd) {
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void mouse_input(RAWINPUT* raw) {
	static POINT p = { 0 };

	GetCursorPos(&p);

	mousePos = vector_sub_2i((vec2i) { p.x, p.y }, mainWindowPosition);

	PT_BINDABLE_EVENT_fire(&e_mouseMove, NULL);
}