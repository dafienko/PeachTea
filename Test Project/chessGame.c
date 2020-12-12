#define _CRT_SECURE_NO_WARNINGS

#include "chessGame.h"
#include "chessSprites.h"

#include <stdlib.h>
#include <stdio.h>

CHESS_GAME* currentGame;

CHESS_SPRITES get_chess_sprites_from_row(PT_IMAGE spriteMap, int row) {
	CHESS_SPRITES sprites = { 0 };

	vec2i unitSize = (vec2i){ 1, 1 };

	sprites.king = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 0, row }, unitSize, 166);
	sprites.queen = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 1, row }, unitSize, 166);
	sprites.bishop = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 2, row }, unitSize, 166);
	sprites.knight = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 3, row }, unitSize, 166);
	sprites.rook = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 4, row }, unitSize, 166);
	sprites.pawn = PT_IMAGE_index_spritemap(spriteMap, (vec2i) { 5, row }, unitSize, 166);
	
	return sprites;
}

CHESS_PIECE* get_piece_from_obj(PT_GUI_OBJ* obj, CHESS_GAME* game) {
	for (int i = 0; i < 16; i++) {
		CHESS_PIECE* piece = (game->whiteTeam->pieces + i);
		PT_IMAGELABEL* pieceImg = (PT_IMAGELABEL*)piece->pieceInstance->subInstance;
		PT_GUI_OBJ* pieceObj = pieceImg->guiObj;
		if (pieceObj == obj) {
			return piece;
		}
	}

	for (int i = 0; i < 16; i++) {
		CHESS_PIECE* piece = (game->blackTeam->pieces + i);
		PT_IMAGELABEL* pieceImg = (PT_IMAGELABEL*)piece->pieceInstance->subInstance;
		PT_GUI_OBJ* pieceObj = pieceImg->guiObj;
		if (pieceObj == obj) {
			return piece;
		}
	}

	return NULL;
}

void clear_grid_highlights() {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			CHESS_SQUARE chessSquare = *(currentGame->chessSquares + y * 8 + x);
			chessSquare.squareDot->visible = 0;
		}
	}
}

void show_paths(PATH* paths, int numPaths) {
	clear_grid_highlights();

	for (int i = 0; i < numPaths; i++) {
		PATH path = *(paths + i);
		PATH_NODE* node = path.origin;

		while (node != NULL) {
			if (node != path.origin) {
				CHESS_SQUARE chessSquare = *(currentGame->chessSquares + (node->position.y - 1) * 8 + (node->position.x - 1));
				chessSquare.squareDot->visible = 01;
			}

			node = node->nextNode;
		}
	}
}

CHESS_PIECE* selectedPiece;
void on_piece_activated(CHESS_PIECE* piece) {
	if (piece->alive) {
		PATH* paths = NULL;
		int numPaths = 0;

		get_piece_paths(*piece, &paths, &numPaths);

		show_paths(paths, numPaths);

		PATHs_destroy(paths, numPaths);

		selectedPiece = piece;
	}
}

void move_piece_to_position(CHESS_PIECE* piece, vec2i boardPos) {
	piece->position = boardPos;
	
	Instance* instance = piece->pieceInstance;
	PT_IMAGELABEL* img = instance->subInstance;
	PT_GUI_OBJ* obj = img->guiObj;
	
	obj->position = boardpos_to_rel_dim(boardPos);
}

CHESS_PIECE* get_piece_from_position(vec2i pos) {
	for (int i = 0; i < 32; i++) {
		CHESS_TEAM_SET* set = i % 2 == 0 ? currentGame->whiteTeam : currentGame->blackTeam;
		int j = i / 2;

		CHESS_PIECE* piece = (set->pieces + j);
		if (vector_equal_2i(pos, piece->position) && piece->alive) {
			return piece;
		}
	}
	
	return NULL;
}

int is_valid_move(CHESS_PIECE* piece, vec2i pos) {
	PATH* paths = NULL;
	int numPaths = 0;

	get_piece_paths(*piece, &paths, &numPaths);

	for (int i = 0; i < numPaths; i++) {
		PATH path = *(paths + i);
		PATH_NODE* node = path.origin;

		while (node != NULL) {
			if (node != path.origin) {
				if (vector_equal_2i(pos, node->position)) {
					return 1;
				}
			}

			node = node->nextNode;
		}
	}

	PATHs_destroy(paths, numPaths);

	return 0;
 }

void setCurrentTurnTeam(CHESS_TEAM_SET* team) {
	currentGame->currentTurnTeam = team;

	PT_COLOR borderColor = team == currentGame->blackTeam ? PT_COLOR_fromRGB(0, 0, 0) : PT_COLOR_fromRGB(255, 255, 255);
	
	for (int i = 0; i < 64; i++) {
		CHESS_SQUARE square = *(currentGame->chessSquares + i);
		
		PT_GUI_OBJ* obj = square.squareFrame->guiObj;
		obj->borderColor = borderColor;
	}
}

void switchCurrentTurnTeam() {
	if (currentGame->currentTurnTeam == currentGame->whiteTeam) {
		setCurrentTurnTeam(currentGame->blackTeam);
	}
	else {
		setCurrentTurnTeam(currentGame->whiteTeam);
	}
}

void on_square_activated(void* args) {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)args;
	vec2i boardPos = { 0 };
	for (int i = 0; i < 64; i++) {
		CHESS_SQUARE square = *(currentGame->chessSquares + i);
		if (square.squareFrame->guiObj == obj) {
			boardPos = square.position;
			break;
		}
	}

	CHESS_PIECE* pieceAtPosition = get_piece_from_position(boardPos);

	if (selectedPiece != NULL && is_valid_move(selectedPiece, boardPos)) { // there is a piece currently selected
		if (!pieceAtPosition || pieceAtPosition->parentSet != selectedPiece->parentSet) { // the activated square is empty, or the piece at the activated square is from the opposing team
			move_piece_to_position(selectedPiece, boardPos);

			if (pieceAtPosition) { // if their was a piece at this position, destroy it
				pieceAtPosition->alive = 0;
				Instance* instance = pieceAtPosition->pieceInstance;
				PT_IMAGELABEL* img = instance->subInstance;
				img->visible = 0;
			}

			selectedPiece->numPieceMoves++;
			selectedPiece = NULL; // deselect the piece that just got moved
			switchCurrentTurnTeam(); // swap current team

			clear_grid_highlights();
			return;
		}
	}

	if (pieceAtPosition && pieceAtPosition->parentSet == currentGame->currentTurnTeam) {
		on_piece_activated(pieceAtPosition);
	}
}

CHESS_GAME* create_chess_game(Instance* boardFrame) {
	CHESS_GAME* chessGame = calloc(1, sizeof(CHESS_GAME));

	PT_IMAGE chessSprites = PT_IMAGE_from_png("assets\\images\\chess.png");

	vec2i whiteKingPos = (vec2i){5, 1};
	vec2i whiteDir = (vec2i){ 0, 1 };
	CHESS_SPRITES whiteChessSprites = get_chess_sprites_from_row(chessSprites, 0);

	vec2i blackKingPos = (vec2i){ 4, 8 };
	vec2i blackDir = vector_mul_2i(whiteDir, -1);
	CHESS_SPRITES blackChessSprites = get_chess_sprites_from_row(chessSprites, 1);

	CHESS_TEAM_SET* whiteTeam = create_chess_team_set(
		whiteKingPos,
		whiteDir,
		boardFrame,
		whiteChessSprites
	);

	whiteTeam->parentGame = chessGame;

	CHESS_TEAM_SET* blackTeam = create_chess_team_set(
		blackKingPos,
		blackDir,
		boardFrame,
		blackChessSprites
	);

	blackTeam->parentGame = chessGame;

	chessGame->whiteTeam = whiteTeam;
	chessGame->blackTeam = blackTeam;
	chessGame->boardFrame = boardFrame;

	chessGame->chessSquares = calloc(8 * 8, sizeof(CHESS_SQUARE));
	// create chess board
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			vec2i boardPos = (vec2i){ x + 1, y + 1 };
			CHESS_SQUARE chessSquare = CHESS_SQUARE_new(boardPos, boardFrame);
			*(chessGame->chessSquares + y * 8 + x) = chessSquare;

			PT_BINDABLE_EVENT_bind(&chessSquare.squareFrame->guiObj->e_obj_activated, on_square_activated);
		}
	}

	currentGame = chessGame;
	setCurrentTurnTeam(whiteTeam);

	return chessGame;
}

void CHESS_GAME_update(CHESS_GAME* chessGame) {
	Instance* boardFrameInstance = chessGame->boardFrame;
	PT_GUI_OBJ* boardFrame = (PT_GUI_OBJ*)boardFrameInstance->subInstance;
	vec2i boardTopLeft = boardFrame->lastAbsoluteDim.position;
	vec2i boardSize = boardFrame->lastAbsoluteDim.size;
	int cellWidth = boardSize.x / 10;
}