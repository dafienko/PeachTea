#ifndef PT_BINDABLE_EVENT_H
#define PT_BINDABLE_EVENT_H

typedef void (*PT_CALLBACK_FUNCTION_POINTER)(void*);

typedef struct {
	PT_CALLBACK_FUNCTION_POINTER* callbacks;
	int numCallbacks;
	int callbackSpace;
	int initialized;
} PT_BINDABLE_EVENT;

void PT_BINDABLE_EVENT_bind(PT_BINDABLE_EVENT* event, PT_CALLBACK_FUNCTION_POINTER callback);
void PT_BINDABLE_EVENT_fire(PT_BINDABLE_EVENT* event, void* args);
void PT_BINDABLE_EVENT_unbind(PT_BINDABLE_EVENT* event, PT_CALLBACK_FUNCTION_POINTER callback);
void PT_BINDABLE_EVENT_destroy(PT_BINDABLE_EVENT* event);

#endif