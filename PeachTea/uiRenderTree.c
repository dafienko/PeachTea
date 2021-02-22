#include "uiRenderTree.h"
#include "sort.h"
#include "Instance.h"
#include "guiObj.h"
#include "scrollFrame.h"

#include <stdio.h>
#include <stdlib.h>

PT_UI_RENDER_TREE* create_render_tree_from_instance(Instance* instance, Z_SORTING_TYPE sortingType);

int comp_ui_render_tree(void* a, void* b) { // for sorting render trees by zindex
	PT_UI_RENDER_TREE* uirtA = *(PT_UI_RENDER_TREE**)a;
	PT_UI_RENDER_TREE* uirtB = *(PT_UI_RENDER_TREE**)b;

	int za = get_instance_zindex(uirtA->rootInstance);
	int zb = get_instance_zindex(uirtB->rootInstance);

	if (za > zb) {
		return 1;
	} 
	else if (za < zb) {
		return -1;
	}
	else {
		return 0;
	}
}

PT_UI_RENDER_TREE* create_render_tree_from_instance(Instance* instance, Z_SORTING_TYPE sortingType) {
	PT_UI_RENDER_TREE* tree = calloc(1, sizeof(PT_UI_RENDER_TREE));
	tree->rootInstance = instance;

	switch (sortingType) {
	case ZST_GLOBAL: // no recursion when global, root instance is assumed to be screenui, branches contain every descendant of screen ui
	{
		unsigned int numDescendants = 0u;
		Instance** descendants = get_descendants(instance, &numDescendants);

		tree->branchSpace = numDescendants;
		tree->numBranches = 0;

		if (numDescendants > 0) {
			tree->branches = calloc(tree->branchSpace, sizeof(PT_UI_RENDER_TREE*));

			for (int i = 0; i < numDescendants; i++) {
				Instance* descendant = *(descendants + i);

				PT_UI_RENDER_TREE* branch = calloc(1, sizeof(PT_UI_RENDER_TREE));
				branch->rootInstance = descendant;

				*(tree->branches + tree->numBranches) = branch;

				tree->numBranches++;
			}

			free(descendants);
		}

		break;
	}
	case ZST_SIBLING: // recursively add direct children of root instance to tree's branches, once done sort branches by child z-index
	{
		int numChildren = get_num_children(instance);

		tree->branchSpace = numChildren;
		tree->numBranches = 0;

		if (tree->branchSpace > 0) {
			tree->branches = calloc(tree->branchSpace, sizeof(PT_UI_RENDER_TREE*));

			for (int i = 0; i < numChildren; i++) {
				Instance* child = *(instance->children + i);

				if (IS_UI_INSTANCE(child->instanceType)) {
					*(tree->branches + tree->numBranches) = create_render_tree_from_instance(child, sortingType);
					tree->numBranches++;
				}
			}
		}

		break;
	}
	}

	if (tree->numBranches > 1) {
		// sort branches in ascending order of ZIndex, such that the lowest ZIndex objects are rendered first, 
		// and the highest are rendered after and atop the lowest
		quicksort((byte*)tree->branches, sizeof(PT_UI_RENDER_TREE*), tree->numBranches, comp_ui_render_tree);
	}

	return tree;
}

PT_UI_RENDER_TREE* PT_UI_RENDER_TREE_generate(PT_SCREEN_UI* screenUi) {
	PT_UI_RENDER_TREE* tree = create_render_tree_from_instance(screenUi->instance, screenUi->sortingType);

	return tree;
}

void PT_UI_RENDER_TREE_render(PT_UI_RENDER_TREE* tree, PT_SCREEN_UI* ui) {
	if (IS_UI_INSTANCE(tree->rootInstance->instanceType)) {
		Instance* instance = tree->rootInstance;
		render_gui_instance(tree->rootInstance, ui);
	}

	for (int i = 0; i < tree->numBranches; i++) {
		PT_UI_RENDER_TREE* branch = *(tree->branches + i);
		PT_UI_RENDER_TREE_render(branch, ui);
	}

	// draw scrollframe ui controls atop descendants
	if (tree->rootInstance->instanceType == IT_SCROLLFRAME) {
		PT_SCROLLFRAME* scrollFrame = (PT_SCROLLFRAME*)tree->rootInstance->subInstance;
		if (scrollFrame->guiObj->visible) {
			PT_GUI_OBJ_render(scrollFrame->vscrollTrack, ui);
			PT_GUI_OBJ_render(scrollFrame->vscrollBar, ui);

			PT_GUI_OBJ_render(scrollFrame->hscrollTrack, ui);
			PT_GUI_OBJ_render(scrollFrame->hscrollBar, ui);
		}
	}
}

void PT_UI_RENDER_TREE_destroy(PT_UI_RENDER_TREE* tree) {
	for (int i = 0; i < tree->numBranches; i++) {
		PT_UI_RENDER_TREE* childTree = *(tree->branches + i);
		PT_UI_RENDER_TREE_destroy(childTree);
	}

	if (tree->branches != NULL) {
		free(tree->branches);
	}

	free(tree);
}