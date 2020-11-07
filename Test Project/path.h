#ifndef CHESS_PATH_H
#define CHESS_PATH_H

#include "PeachTea.h"

typedef struct {
	vec2i position;
	struct PATH_NODE* nextNode;
} PATH_NODE;

typedef struct {
	PATH_NODE* origin;
	int length;
} PATH;

#endif