#ifndef CHESS_SQUARE_H
#define CHESS_SQUARE_H

#include "PeachTea.h"

typedef struct {
	vec2i position;
	PT_IMAGELABEL* squareFrame;
	PT_IMAGELABEL* squareDot;
} CHESS_SQUARE;

CHESS_SQUARE CHESS_SQUARE_new(vec2i pos, Instance* boardFrame);

#endif