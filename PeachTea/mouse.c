#include "mouse.h"
#include "screenSize.h"

#include <stdio.h>

RAWINPUTDEVICE Rid[1] = { 0 };
HWND hMainWnd;

void mouse_init(HWND hWnd) {
	hMainWnd = hWnd;

	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hMainWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void mouse_input(RAWINPUT* raw) {
	static POINT p = { 0 };

	GetCursorPos(&p);

	mousePos = vector_sub_2i((vec2i) { p.x, p.y }, mainWindowPosition);

	//mousePos = (vec2i){ raw->data.mouse.lLastX, raw->data.mouse.lLastY };

	PT_BINDABLE_EVENT_fire(&e_mouseMove, NULL);
}

void mouse_button1_down() {
	PT_BINDABLE_EVENT_fire(&e_mouseDown, NULL);
}

void mouse_button2_up() {
	PT_BINDABLE_EVENT_fire(&e_mouseUp, NULL);
}