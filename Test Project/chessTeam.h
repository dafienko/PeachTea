#ifndef CHESS_TEAM_H
#define CHESS_TEAM_H

#include "PeachTea.h"
#include "chessSprites.h"
#include "path.h"

typedef enum {
	CHESS_PIECE_PAWN,
	CHESS_PIECE_ROOK,
	CHESS_PIECE_KNIGHT,
	CHESS_PIECE_BISHOP,
	CHESS_PIECE_KING,
	CHESS_PIECE_QUEEN
} CHESS_PIECE_TYPE;

typedef struct {
	CHESS_PIECE_TYPE pieceType;

	struct CHESS_TEAM_SET* parentSet;

	vec2i position;
	BOOL alive;
	int numPieceMoves;

	Instance* pieceInstance;

	void(*getPaths)(struct CHESS_TEAM_SET, PATH*, int*);
} CHESS_PIECE;

typedef struct {
	struct CHESS_GAME* parentGame;

	vec2i forwardDirection; // not always relevant, but some pieces need it
	
	CHESS_PIECE* pieces;
} CHESS_TEAM_SET;

CHESS_TEAM_SET* create_chess_team_set(const vec2i kingPosition, const vec2i forwardDirection, const Instance* boardFrame, const CHESS_SPRITES chessSprites);
int is_piece_at_position(CHESS_TEAM_SET* set, vec2i pos);
void get_piece_paths(const CHESS_PIECE* piece, PATH** pathsOut, int* numPathsOut);

#endif