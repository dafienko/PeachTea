#ifndef PT_INSTANCE_H
#define PT_INSTANCE_H

#include "BindableEvent.h"

typedef enum {
	IT_UNDEF,

	IT_GUI_OBJ,
	IT_TEXTLABEL,
	IT_IMAGELABEL,
	IT_SCROLLFRAME,

	IT_SCREEN_UI
} INSTANCE_TYPE;

#define IS_UI_INSTANCE(IT) IT >= 1 && IT <= 4

typedef struct Instance {
	INSTANCE_TYPE instanceType;
	void* subInstance; 
	void (*destroySubInstance)(void*);

	char* name;
	
	struct Instance** children;
	struct Instance* parent;
	int numChildren;
	int childrenArraySize;

	PT_BINDABLE_EVENT childAdded;
	PT_BINDABLE_EVENT childRemoved;
} Instance;

void set_instance_parent(Instance* i, Instance* newParent);
Instance* new_instance();
void destroy_instance(Instance* i);
Instance* get_child_from_name(Instance* parent, const char* name);
Instance* clone_instance(Instance* source);

#endif