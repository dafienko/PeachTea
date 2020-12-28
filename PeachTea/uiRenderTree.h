#ifndef PT_UI_RENDER_TREE_H
#define PT_UI_RENDER_TREE_H

#include "Instance.h"
#include "ScreenUI.h"
#include "glExtensions.h"

typedef struct {
	Instance* rootInstance;

	struct PT_UI_RENDER_TREE** branches;
	int numBranches;
	int branchSpace;
} PT_UI_RENDER_TREE;

PT_UI_RENDER_TREE* PT_UI_RENDER_TREE_generate(PT_SCREEN_UI* screenUi);
void PT_UI_RENDER_TREE_render(PT_UI_RENDER_TREE* tree, PT_SCREEN_UI* screenUi);
void PT_UI_RENDER_TREE_destroy(PT_UI_RENDER_TREE* tree);

#endif