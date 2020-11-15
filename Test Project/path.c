#include "path.h"
#include <stdlib.h>


void PATH_add_node_to_path(PATH* path, vec2i nodePosition) {
	PATH_NODE* node = calloc(1, sizeof(PATH_NODE));
	node->nextNode = NULL;
	node->position = nodePosition;

	path->length++;
	if (path->origin == NULL) {
		path->origin = node;
	}
	else {
		path->terminalNode->nextNode = node;
	}

	path->length++;
	path->terminalNode = node;
}

void PATHs_destroy(PATH* paths, int numPaths) {
	for (int i = 0; i < numPaths; i++) {
		PATH path = *(paths + i);

		PATH_NODE* node = path.origin;
		path.origin = NULL;

		while (node != NULL) {
			PATH_NODE* nextNode = node->nextNode;

			free(node);

			node = nextNode;
		}

		path.terminalNode = NULL;
	}

	free(paths);
}