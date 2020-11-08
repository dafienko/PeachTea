#include "instance.h"
#include <stdlib.h>
#include <string.h>

void set_instance_parent(Instance* i, Instance* newParent) {
	if (i->parent != NULL) { // remove instance from old parent
		Instance* oldParent = i->parent;

		int numChildren = i->parent->numChildren;
		int currentChildrenArraySize = i->parent->childrenArraySize;
		numChildren--; // a child is being removed: decrement numChildren
		i->parent->numChildren = numChildren;

		// replace pointer to this instance with pointer to a valid child in the array
		for (int k = 0; k < numChildren; k++) {
			if (i->parent->children + k == i) {
				*(i->parent->children + k) = *(i->parent->children + numChildren + 1);
				*(i->parent->children + numChildren + 1) = NULL;
				break;
			}
		}

		if (numChildren < currentChildrenArraySize / 2 && numChildren > 2) { // shrink-wrap children pointer size to only the pointers that are required
			i->parent->children = realloc(i->parent->children, (currentChildrenArraySize / 2) * sizeof(Instance*));
			i->parent->childrenArraySize = currentChildrenArraySize / 2;
		}

		PT_BINDABLE_EVENT_fire(&oldParent->childRemoved, oldParent);
	}

	i->parent = newParent;

	if (newParent != NULL) { // add instance to new parent children array

		if (newParent->numChildren + 1 > newParent->childrenArraySize) { // expand children array in case the current one isn't big enough
			newParent->childrenArraySize *= 2;
			newParent->children = realloc(newParent->children, newParent->childrenArraySize * sizeof(Instance*));
		}

		*(newParent->children + newParent->numChildren) = i;

		newParent->numChildren++;

		PT_BINDABLE_EVENT_fire(&newParent->childAdded, newParent);
	}
}

Instance* new_instance() {
	Instance* i = (Instance*)calloc(1, sizeof(Instance));
	i->children = calloc(2, sizeof(Instance*));
	i->parent = NULL;
	i->childrenArraySize = 2;
	i->numChildren = 0;
	i->subInstance = NULL;
	i->name = "";
	
	return i;
}

Instance* get_child_from_name(Instance* parent, const char* name) {
	for (int i = 0; i < parent->numChildren; i++) {
		Instance* child = *(parent->children + i);
		char* childName = child->name;

		if (childName && strcmp(childName, name) == 0) {
			return child;
		}
	}

	return NULL;
}

void destroy_instance(Instance* instance) {
	instance->destroySubInstance(instance->subInstance);

	if (instance->name) {
		free(instance->name);
	}

	free(instance); 
}