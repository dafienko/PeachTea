#include "instance.h"
#include <stdlib.h>
#include <string.h>

#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "ScreenUI.h"

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

void init_instance(Instance* i) {
	i->children = calloc(2, sizeof(Instance*));
	i->parent = NULL;
	i->childrenArraySize = 2;
	i->numChildren = 0;
}

Instance* new_instance() {
	Instance* i = (Instance*)calloc(1, sizeof(Instance));
	init_instance(i);
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

Instance* clone_instance(Instance* source) {
	Instance* clone = calloc(1, sizeof(Instance));
	memcpy(clone, source, sizeof(Instance));

	if (source->name != NULL) {
		int nameLen = strlen(source->name) + 5;
		char* name = calloc(nameLen, sizeof(char));
		memcpy(name, source->name, nameLen * sizeof(char));
		clone->name = name;
	}

	clone->parent = NULL;
	init_instance(clone);

	void* subInstance = NULL;
	switch (source->instanceType) {
	case IT_GUI_OBJ:
		subInstance = (void*)PT_GUI_OBJ_clone((PT_GUI_OBJ*)source->subInstance, clone);
		break;
	case IT_IMAGELABEL:
		subInstance = (void*)PT_IMAGELABEL_clone((PT_IMAGELABEL*)source->subInstance, clone);
		break;
	case IT_TEXTLABEL:
		subInstance = (void*)PT_TEXTLABEL_clone((PT_TEXTLABEL*)source->subInstance, clone);
		break;
	case IT_SCREEN_UI:
		subInstance = (void*)PT_SCREEN_UI_clone((PT_SCREEN_UI*)source->subInstance, clone);
		break;
	}

	clone->subInstance = subInstance;

	return clone;
}

void destroy_instance(Instance* instance) {
	instance->destroySubInstance(instance->subInstance);

	if (instance->name) {
		free(instance->name);
	}

	free(instance); 
}

unsigned int get_num_children(Instance* instance) {
	return instance->numChildren;
}

unsigned int get_num_descendants(Instance* instance) {
	int n = instance->numChildren;

	for (int i = 0; i < instance->numChildren; i++) {
		Instance* child = *(instance->children + i);
		n += get_num_descendants(child);
	}

	return n;
}

Instance** get_descendants(Instance* parent, unsigned int* numDescendantsOut) {
	unsigned int numDescendants = get_num_descendants(parent);
	*numDescendantsOut = numDescendants;

	Instance** descendants = NULL;

	if (numDescendants > 0) {
		descendants = calloc(numDescendants, sizeof(Instance*));
	}

	int descCount = 0;

	for (int i = 0; i < parent->numChildren; i++) {
		Instance* child = *(parent->children + i);

		*(descendants + descCount) = child;
		descCount++;

		unsigned int numChildDescendants = 0u;
		Instance** childDescendants = get_descendants(child, &numChildDescendants);

		if (numChildDescendants > 0) {
			memcpy(descendants + descCount, childDescendants, numChildDescendants * sizeof(Instance*));
			free(childDescendants);

			descCount += numChildDescendants;
		}
	}

	return descendants;
}