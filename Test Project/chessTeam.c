#include "chessTeam.h"
#include "chessSprites.h"
#include "chessGame.h"
#include <stdlib.h>
#include <math.h>

#include "PeachTea.h"
#pragma comment(lib, "PeachTea")

int is_pos_on_board(vec2i pos) {
	return pos.x > 0 && pos.x <= 8 && pos.y > 0 && pos.y <= 8;
}

CHESS_TEAM_SET* get_opposing_set(CHESS_TEAM_SET* set) {
	CHESS_GAME* parentGame = set->parentGame;

	if (parentGame->whiteTeam == set) {
		return parentGame->blackTeam;
	}
	else {
		return parentGame->whiteTeam;
	}
}

int is_empty_position(CHESS_GAME* game, vec2i pos) {
	return !is_piece_at_position(game->whiteTeam, pos) && !is_piece_at_position(game->blackTeam, pos);
}

PATH* get_path_from_dir(PATH* pathOut, CHESS_TEAM_SET* set, vec2i origin, vec2i dir, int len) { // if len is <= 0, len is interpreted as infinite
	CHESS_GAME* game = set->parentGame;
	CHESS_TEAM_SET* opposingSet = get_opposing_set(set);

	vec2i pos = origin;

	int d = 0;

	// keep "walking" until the next step is off the board, on another piece, or maximum distance is 1 step away
	do {
		PATH_add_node_to_path(pathOut, pos);

		vector_add_2i(pos, dir);

		d++;
	} while (is_pos_on_board(pos) && is_empty_position(game, pos) && (len <= 0 || d < len));

	// if the last step is on an enemy piece or on an empty position, add it to the path 
	if (is_piece_at_position(opposingSet, pos) || (is_empty_position(game, pos) && is_pos_on_board(pos))) {
		PATH_add_node_to_path(pathOut, pos);
	}
}

void get_paths_pawn(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 3;
	PATH* paths = calloc(numPaths, sizeof(PATH));

	CHESS_TEAM_SET* parentSet = piece.parentSet;
	vec2i origin = piece.position;
	vec2i forwardDir = parentSet->forwardDirection;
	vec2i rightDir = (vec2i){ forwardDir.y, -forwardDir.x };

	CHESS_TEAM_SET* opposingSet = get_opposing_set(parentSet);

	for (int i = 0; i < numPaths; i++) {
		PATH* path = paths + i;
		PATH_add_node_to_path(path, origin); // every path starts with origin at piece's current position

		switch (i) {
		case 0: // middle "forward" path
			;
			vec2i f1 = vector_add_2i(origin, forwardDir);

			int onBoard = is_pos_on_board(f1);
			int isEmpty = is_empty_position(parentSet->parentGame, f1);

			if (is_pos_on_board(f1) && is_empty_position(parentSet->parentGame, f1)) {
				PATH_add_node_to_path(path, f1);

				vec2i f2 = vector_add_2i(f1, forwardDir);
				if (piece.numPieceMoves == 0 && is_empty_position(parentSet->parentGame, f2)) { // pawn can move forward twice on first move
					PATH_add_node_to_path(path, f2);
				}
			}

			break;
		case 1: // left "attack" path
			;
			vec2i laPos = vector_add_2i(origin, vector_sub_2i(forwardDir, rightDir));
			if (is_piece_at_position(opposingSet, laPos)) {
				PATH_add_node_to_path(path, laPos);
			}
			break;
		case 2: // right "attack" path
			;
			vec2i raPos = vector_add_2i(origin, vector_add_2i(forwardDir, rightDir));
			if (is_piece_at_position(opposingSet, raPos)) {
				PATH_add_node_to_path(path, raPos);
			}
			break;
		}
	}

	

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

void get_paths_king(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 8;
	PATH* paths = calloc(numPaths, sizeof(PATH));
	
	vec2i origin = piece.position;
	CHESS_TEAM_SET* set = piece.parentSet;

	vec2i kingDirs[8] = {
		(vec2i) {-1, -1}, (vec2i) {0, -1}, (vec2i) {1, -1},
		(vec2i) {-1, 0},                   (vec2i) {1, 0},
		(vec2i) {-1, 1},  (vec2i) {0, 1},  (vec2i) {1, 1},
	};

	for (int i = 0; i < numPaths; i++) {
		vec2i dir = kingDirs[i];
		PATH* path = paths + i;

		PATH_add_node_to_path(path, origin);

		vec2i pos = vector_add_2i(origin, dir);
		if (is_pos_on_board(pos) && !is_piece_at_position(set, pos)) {
			PATH_add_node_to_path(path, pos);
		}
	}

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

void get_paths_queen(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 8;
	PATH* paths = calloc(numPaths, sizeof(PATH));

	vec2i origin = piece.position;

	vec2i queenDirs[8] = {
		(vec2i) {-1, -1}, (vec2i) {0, -1}, (vec2i) {1, -1},
		(vec2i) {-1, 0},                   (vec2i) {1, 0},
		(vec2i) {-1, 1},  (vec2i) {0, 1},  (vec2i) {1, 1},
	};

	for (int i = 0; i < 8; i++) {
		PATH* path = paths + i;
		vec2i dir = queenDirs[i];

		get_path_from_dir(path, piece.parentSet, origin, dir, 0);
	}

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

void get_paths_rook(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 4;
	PATH* paths = calloc(numPaths, sizeof(PATH));

	vec2i origin = piece.position;

	vec2i queenDirs[4] = {
							(vec2i) {0, -1}, 
		(vec2i) {-1, 0},                   (vec2i) {1, 0},
							(vec2i) {0, 1}
	};

	for (int i = 0; i < 4; i++) {
		PATH* path = paths + i;
		vec2i dir = queenDirs[i];

		get_path_from_dir(path, piece.parentSet, origin, dir, 0);
	}

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

vec2i rotate_clockwise(vec2i v) {
	return (vec2i) { v.y, -v.x };
}

void get_paths_knight(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 8;
	PATH* paths = calloc(numPaths, sizeof(PATH));

	vec2i origin = piece.position;

	CHESS_TEAM_SET* set = piece.parentSet;

	vec2i d1 = (vec2i){ 1, -2 };
	vec2i d2 = (vec2i){ 2, -1 };

	for (int i = 0; i < 4; i++) {
		PATH* path1 = paths + i * 2 + 0;
		PATH* path2 = paths + i * 2 + 1;

		PATH_add_node_to_path(path1, origin);
		PATH_add_node_to_path(path2, origin);

		vec2i p1 = vector_add_2i(origin, d1);
		vec2i p2 = vector_add_2i(origin, d2);

		if (is_pos_on_board(p1) && !is_piece_at_position(set, p1)) {
			PATH_add_node_to_path(path1, p1);
		}

		if (is_pos_on_board(p2) && !is_piece_at_position(set, p2)) {
			PATH_add_node_to_path(path1, p2);
		}

		d1 = rotate_clockwise(d1);
		d2 = rotate_clockwise(d2);
	}

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

void get_paths_bishop(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	int numPaths = 4;
	PATH* paths = calloc(numPaths, sizeof(PATH));

	vec2i origin = piece.position;

	vec2i queenDirs[4] = {
		(vec2i) {-1, -1},					 (vec2i) {1, -1},

		(vec2i) {-1, 1},                     (vec2i) {1, 1}
	};

	for (int i = 0; i < 4; i++) {
		PATH* path = paths + i;
		vec2i dir = queenDirs[i];

		get_path_from_dir(path, piece.parentSet, origin, dir, 0);
	}

	*pathsOut = paths;
	*numPathsOut = numPaths;
}

void get_piece_paths(const CHESS_PIECE piece, PATH** pathsOut, int* numPathsOut) {
	switch (piece.pieceType) {
	case CHESS_PIECE_PAWN:
		get_paths_pawn(piece, pathsOut, numPathsOut);
		break;
	case CHESS_PIECE_ROOK:
		get_paths_rook(piece, pathsOut, numPathsOut);
		break;
	case CHESS_PIECE_KNIGHT:
		get_paths_knight(piece, pathsOut, numPathsOut);
		break;
	case CHESS_PIECE_BISHOP:
		get_paths_bishop(piece, pathsOut, numPathsOut);
		break;
	case CHESS_PIECE_KING:
		get_paths_king(piece, pathsOut, numPathsOut);
		break;
	case CHESS_PIECE_QUEEN:
		get_paths_queen(piece, pathsOut, numPathsOut);
		break;
	}
}

PT_REL_DIM boardpos_to_rel_dim(const vec2i boardpos) {
	return PT_REL_DIM_new((boardpos.x - 1) / 8.0f, 0, (boardpos.y - 1) / 8.0f, 0);
}

CHESS_TEAM_SET* create_chess_team_set(const vec2i kingPosition, const vec2i forwardDirection, const Instance* boardFrame, const CHESS_SPRITES chessSprites) {
	CHESS_TEAM_SET* set = calloc(1, sizeof(CHESS_TEAM_SET));

	set->forwardDirection = forwardDirection; // not always relevant, but some pieces need it
	set->pieces = calloc(16, sizeof(CHESS_PIECE));

	vec2i rightDirection = (vec2i){forwardDirection.y, -forwardDirection.x};

	// create pawns
	vec2i pawn0Pos = vector_add_2i(kingPosition, vector_add_2i(forwardDirection, vector_mul_2i(rightDirection, -4)));
	for (int i = 0; i < 8; i++) {
		CHESS_PIECE pawn = { 0 };

		vec2i pawnPos = vector_add_2i(pawn0Pos, vector_mul_2i(rightDirection, i));

		Instance* pieceInstance = PT_IMAGELABEL_new();

		PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)pieceInstance->subInstance;
		imageLabel->image = chessSprites.pawn;

		PT_GUI_OBJ* guiObj = imageLabel->guiObj;
		guiObj->size = PT_REL_DIM_new(.125f, 0, .125f, 0); // square 1/8 x 1/8 of the total board size
		guiObj->position = boardpos_to_rel_dim(pawnPos);
		guiObj->zIndex = 2;
		guiObj->backgroundTransparency = 1;
		guiObj->borderWidth = 0;

		set_instance_parent(pieceInstance, boardFrame);

		pawn.alive = TRUE;
		pawn.getPaths = get_paths_pawn;
		pawn.numPieceMoves = 0;
		pawn.pieceInstance = pieceInstance;
		pawn.position = pawnPos;
		pawn.pieceType = CHESS_PIECE_PAWN;
		pawn.parentSet = set;
		
		*(set->pieces + i) = pawn;
	}

	// create other pieces
	vec2i piece0Pos = vector_add_2i(kingPosition, vector_mul_2i(rightDirection, -4));
	for (int i = 0; i < 8; i++) {
		CHESS_PIECE piece = { 0 };

		vec2i pos = vector_add_2i(piece0Pos, vector_mul_2i(rightDirection, i));

		Instance* pieceInstance = PT_IMAGELABEL_new();

		PT_IMAGELABEL* imageLabel = (PT_IMAGELABEL*)pieceInstance->subInstance;

		PT_GUI_OBJ* guiObj = imageLabel->guiObj;
		guiObj->size = PT_REL_DIM_new(.125f, 0, .125f, 0); // square 1/8 x 1/8 of the total board size
		guiObj->position = boardpos_to_rel_dim(pos);
		guiObj->zIndex = 2;
		guiObj->backgroundTransparency = 1;
		guiObj->borderWidth = 0;

		set_instance_parent(pieceInstance, boardFrame);

		piece.alive = TRUE;
		piece.getPaths = get_paths_pawn;
		piece.numPieceMoves = 0;
		piece.pieceInstance = pieceInstance;
		piece.position = pos;
		piece.parentSet = set;
		
		int distFromSide = lroundf(fabs(3.5f - (float)i) + 0.5f);
		switch (distFromSide) {
		case 4:
			piece.pieceType = CHESS_PIECE_ROOK;
			imageLabel->image = chessSprites.rook;
			break;
		case 3:
			piece.pieceType = CHESS_PIECE_KNIGHT;
			imageLabel->image = chessSprites.knight;
			break;
		case 2:
			piece.pieceType = CHESS_PIECE_BISHOP;
			imageLabel->image = chessSprites.bishop;
			break;
		case 1:
			if (vector_equal_2i(kingPosition, pos)) {
				piece.pieceType = CHESS_PIECE_KING;
				imageLabel->image = chessSprites.king;
			}
			else {
				piece.pieceType = CHESS_PIECE_QUEEN;
				imageLabel->image = chessSprites.queen;
			}
			break;
		}

		*(set->pieces + 8 + i) = piece;
	}

	return set;
}

int is_piece_at_position(CHESS_TEAM_SET* set, vec2i pos) {
	for (int i = 0; i < 16; i++) {
		CHESS_PIECE piece = *(set->pieces + i);

		if (piece.alive) {
			if (vector_equal_2i(piece.position, pos)) {
				return 1;
			}
		}
	}

	return 0;
}