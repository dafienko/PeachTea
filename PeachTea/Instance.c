#include "instance.h"
#include <stdlib.h>
#include <string.h>

#include "guiObj.h"
#include "imageLabel.h"
#include "textLabel.h"
#include "ScreenUI.h"
#include "expandableArray.h"

void set_instance_parent(Instance* instance, Instance* newParent) {
	if (instance->parent != NULL) { // remove instance from old parent
		Instance* oldParent = instance->parent;

		for (int i = 0; i < oldParent->children.numElements; i++) {
			Instance* child = *(Instance**)PT_EXPANDABLE_ARRAY_get(&oldParent->children, i);
			if (child == instance) {
				PT_EXPANDABLE_ARRAY_remove(&oldParent->children, i);
				break;
			}
		}

		PT_BINDABLE_EVENT_fire(&oldParent->childRemoved, oldParent);
	}

	instance->parent = newParent;

	if (newParent != NULL) { // add instance to new parent children array
		PT_EXPANDABLE_ARRAY_add(&newParent->children, &instance);

		PT_BINDABLE_EVENT_fire(&newParent->childAdded, newParent);
	}
}

void init_instance(Instance* i) {
	i->children = PT_EXPANDABLE_ARRAY_new(1, sizeof(Instance*));
	i->parent = NULL;
}

Instance* new_instance() {
	Instance* i = (Instance*)calloc(1, sizeof(Instance));
	init_instance(i);
	i->subInstance = NULL;
	i->name = NULL;
	
	return i;
}

Instance* get_child_from_name(Instance* parent, const char* name) {
	for (int i = 0; i < parent->children.numElements; i++) {
		Instance* child = *(Instance**)PT_EXPANDABLE_ARRAY_get(&parent->children, i);
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
	if (instance->parent) {
		set_instance_parent(instance, NULL);
	}

	for (int i = 0; i < instance->children.numElements; i++) {
		Instance* child = *(Instance**)PT_EXPANDABLE_ARRAY_get(&instance->children, i);
		destroy_instance(child);
	}
	
	if (instance->name) {
		free(instance->name);
	}
	
	instance->destroySubInstance(instance->subInstance);

	PT_BINDABLE_EVENT_destroy(&instance->childAdded);
	PT_BINDABLE_EVENT_destroy(&instance->childRemoved);


	free(instance); 
}

unsigned int get_num_children(Instance* instance) {
	return instance->children.numElements;
}

unsigned int get_num_descendants(Instance* instance) {
	int n = get_num_children(instance);

	for (int i = 0; i < n; i++) {
		Instance* child = *(Instance**)PT_EXPANDABLE_ARRAY_get(&instance->children, i);
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

	for (int i = 0; i < get_num_children(parent); i++) {
		Instance* child = *(Instance**)PT_EXPANDABLE_ARRAY_get(&parent->children, i);

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