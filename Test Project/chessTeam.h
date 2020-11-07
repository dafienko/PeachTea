#ifndef CHESS_TEAM_H
#define CHESS_TEAM_H

#include "PeachTea.h"

typedef enum {
	CHESS_PIECE_PAWN,
	CHESS_PIECE_ROOK,
	CHESS_PIECE_KNIGHT,
	CHESS_PIECE_BISHOP,
	CHESS_PICEE_KING,
	CHESS_PIECE_QUEEN
} CHESS_PIECE_TYPE;


typedef struct {
	vec2i forwardDirection; // not always relevant, but some pieces need it
	struct CHESS_PIECE* pieces;
} CHESS_TEAM_SET;

typedef struct {
	CHESS_PIECE_TYPE pieceType;

	vec2i position;
	
	
} CHESS_PIECE;


#endif