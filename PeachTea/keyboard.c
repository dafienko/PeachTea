#include "keyboard.h"
#include "lightWindows.h"

int is_key_down(int key) {
	return GetKeyState(key) < 0;
}