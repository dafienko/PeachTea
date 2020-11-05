#ifndef PT_INSTANCE_H
#define PT_INSTANCE_H

typedef enum {
	IT_UNDEF,

	IT_GUI_OBJ,
	IT_TEXTLABEL,
	IT_IMAGELABEL,

	IT_SCREEN_UI
} INSTANCE_TYPE;

#define IS_UI_INSTANCE(IT) IT >= 1 && IT <= 3

typedef struct Instance {
	INSTANCE_TYPE instanceType;
	void* subInstance; 
	void (*destroySubInstance)(void*);

	char* name;
	
	struct Instance** children;
	struct Instance* parent;
	int numChildren;
	int childrenArraySize;
} Instance;

void set_instance_parent(Instance* i, Instance* newParent);
Instance* new_instance();
void destroy_instance(Instance* i);

#endif