#ifndef PT_KEYBOARD_H
#define PT_KEYBOARD_H

#include "BindableEvent.h"

PT_BINDABLE_EVENT eOnKeyPress, eOnKeyRelease; // arg type is (void*)&int    where int is a windows virtual keycode
PT_BINDABLE_EVENT eOnSysKeyPress, eOnSysKeyRelease; // arg type is (void*)&int    where int is a windows virtual keycode
PT_BINDABLE_EVENT eOnCharTyped; // arg type is (void*)&char

int is_key_down(int key);

#endif