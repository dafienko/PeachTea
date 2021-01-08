#ifndef PT_INSTANCE_H
#define PT_INSTANCE_H

#include "BindableEvent.h"

typedef enum {
	IT_UNDEF,

	IT_GUI_OBJ,
	IT_TEXTLABEL,
	IT_IMAGELABEL,
	IT_SCROLLFRAME,
	IT_RENDERFRAME,

	IT_SCREEN_UI
} INSTANCE_TYPE;

#define IS_UI_INSTANCE(IT) IT > IT_UNDEF && IT < IT_SCREEN_UI

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

unsigned int get_num_children(Instance* instance);
unsigned int get_num_descendants(Instance* instance);
Instance** get_descendants(Instance* parent, unsigned int* numDescendantsOut);

#endif