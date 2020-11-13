#ifndef CHESS_PATH_H
#define CHESS_PATH_H

#include "PeachTea.h"

typedef struct {
	vec2i position;
	struct PATH_NODE* nextNode;
} PATH_NODE;

typedef struct {
	PATH_NODE* origin;
	PATH_NODE* terminalNode;
	int length;
} PATH;

void PATH_add_node_to_path(PATH* path, vec2i nodePosition);
void PATHs_destroy(PATH* paths, int numPaths);

#endif