#ifndef PT_INSTANCE_H
#define PT_INSTANCE_H

typedef struct Instance {
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