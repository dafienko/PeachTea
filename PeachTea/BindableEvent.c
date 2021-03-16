#include "BindableEvent.h"
#include <stdlib.h>

void PT_BINDABLE_EVENT_init(PT_BINDABLE_EVENT* event) {
	*event = (PT_BINDABLE_EVENT){ 0 };

	event->callbackSpace = 2;
	event->callbacks = calloc(event->callbackSpace, sizeof(PT_CALLBACK_FUNCTION_POINTER));
	event->initialized = 1;
}

void PT_BINDABLE_EVENT_bind(PT_BINDABLE_EVENT* event, PT_CALLBACK_FUNCTION_POINTER callback) {

	if (!event->initialized) {
		PT_BINDABLE_EVENT_init(event);
	}

	if (event->numCallbacks + 1 > event->callbackSpace) {
		event->callbackSpace *= 2;
		event->callbacks = realloc(event->callbacks, event->callbackSpace * sizeof(PT_CALLBACK_FUNCTION_POINTER));
	}

	*(event->callbacks + event->numCallbacks) = callback;
	event->numCallbacks++;
}

void PT_BINDABLE_EVENT_fire(PT_BINDABLE_EVENT* event, void* args) {
	if (event->initialized == 1) {
		for (int i = 0; i < event->numCallbacks; i++) {
			PT_CALLBACK_FUNCTION_POINTER callback = *(event->callbacks + i);
			callback(args);
		}
	}
}

void PT_BINDABLE_EVENT_unbind(PT_BINDABLE_EVENT* event, PT_CALLBACK_FUNCTION_POINTER callbackToRemove) {
	int removedIndex = -1;
	for (int i = 0; i < event->numCallbacks; i++) {
		PT_CALLBACK_FUNCTION_POINTER callback = *(event->callbacks + i);
		if (callback == callbackToRemove) {
			removedIndex = i;
			break;
		}
	}

	if (removedIndex >= 0) {
		*(event->callbacks + removedIndex) = NULL;
		event->numCallbacks--;
		
		if (removedIndex != event->numCallbacks) { // replace the callback that got removed with the last callback in the "array"
			*(event->callbacks + removedIndex) = *(event->callbacks + event->numCallbacks);
		}
	}
}

void PT_BINDABLE_EVENT_destroy(PT_BINDABLE_EVENT* event) {
	if (event->initialized) {
		free(event->callbacks);
	}
}