#include "chessTeam.h"
#include "chessSprites.h"
#include <stdlib.h>
#include <math.h>

#include "PeachTea.h"
#pragma comment(lib, "PeachTea")

void get_paths_pawn(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

void get_paths_king(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

void get_paths_queen(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

void get_paths_rook(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

void get_paths_horse(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

void get_paths_bishop(const CHESS_TEAM_SET set, PATH* pathsOut, int* numPathsOut) {

}

PT_REL_DIM boardpos_to_rel_dim(const vec2i boardpos) {
	return PT_REL_DIM_new((boardpos.x - 1) / 8.0f, 0, (boardpos.y - 1) / 8.0f, 0);
}

CHESS_TEAM_SET* create_chess_team_set(const vec2i kingPosition, const vec2i forwardDirection, const Instance* boardFrame, const CHESS_SPRITES chessSprites) {
	CHESS_TEAM_SET* set = calloc(1, sizeof(CHESS_TEAM_SET));

	set->forwardDirection; // not always relevant, but some pieces need it
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
		guiObj->zIndex = 10;
		guiObj->backgroundTransparency = 1;
		guiObj->borderWidth = 0;

		set_instance_parent(pieceInstance, boardFrame);

		pawn.alive = TRUE;
		pawn.getPaths = get_paths_pawn;
		pawn.numPieceMoves = 0;
		pawn.pieceInstance = pieceInstance;
		pawn.position = pawnPos;
		
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
		guiObj->zIndex = 10;
		guiObj->backgroundTransparency = 1;
		guiObj->borderWidth = 0;

		set_instance_parent(pieceInstance, boardFrame);

		piece.alive = TRUE;
		piece.getPaths = get_paths_pawn;
		piece.numPieceMoves = 0;
		piece.pieceInstance = pieceInstance;
		piece.position = pos;

		int distFromSide = lroundf(fabs(3.5f - (float)i) + 0.5f);
		switch (distFromSide) {
		case 4:
			imageLabel->image = chessSprites.rook;
			break;
		case 3:
			imageLabel->image = chessSprites.knight;
			break;
		case 2:
			imageLabel->image = chessSprites.bishop;
			break;
		case 1:
			if (vector_equal_2i(kingPosition, pos)) {
				imageLabel->image = chessSprites.king;
			}
			else {
				imageLabel->image = chessSprites.queen;
			}
			break;
		}

		*(set->pieces + i) = piece;
	}

	return set;
}