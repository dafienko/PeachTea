#include "instance.h"
#include <stdlib.h>

void set_instance_parent(Instance* i, Instance* newParent) {
	if (i->parent != NULL) { // remove instance from old parent
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
	}

	i->parent = newParent;

	if (newParent != NULL) { // add instance to new parent children array
		int numChildren = newParent->numChildren;
		int currentChildrenArraySize = newParent->childrenArraySize;
		numChildren++;
		newParent->numChildren = numChildren;

		if (numChildren >= currentChildrenArraySize) { // expand children array in case the current one isn't big enough
			currentChildrenArraySize *= 2;
			newParent->children = realloc(newParent->children, currentChildrenArraySize * sizeof(Instance*));
		}

		*(newParent->children + numChildren - 1) = i;
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

void destroy_instance(Instance* instance) {
	instance->destroySubInstance(instance->subInstance);
	free(instance); // LOL
}