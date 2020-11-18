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
	for (int i = 0; i < currentGame->boardFrame->numChildren; i++) {
		Instance* child = *(currentGame->boardFrame->children + i);
		if (child->name != "" && child->instanceType == IT_IMAGELABEL) {
			PT_IMAGELABEL* imageLabel = child->subInstance;
			imageLabel->visible = 0;
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
				char* name = calloc(20, sizeof(char));
				sprintf(name, "%i,%i", node->position.x, node->position.y);
				
				Instance* square = get_child_from_name(currentGame->boardFrame, name);
				free(name);

				if (square != NULL) {
					PT_IMAGELABEL* squareImage = square->subInstance;
					squareImage->visible = 1;
				}
			}

			node = node->nextNode;
		}
	}
}

CHESS_PIECE* selectedPiece;
void on_piece_activated(void* args) {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)args;

	CHESS_PIECE* piece = get_piece_from_obj(obj, currentGame);
	
	if (piece->alive) {
		PATH* paths = NULL;
		int numPaths = 0;

		get_piece_paths(*piece, &paths, &numPaths);

		show_paths(paths, numPaths);

		PATHs_destroy(paths, numPaths);
	}
}

void on_square_activated(void* args) {
	PT_GUI_OBJ* obj = (PT_GUI_OBJ*)args;
	
	if (selectedPiece != NULL) {

	}
	else {
		
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

	for (int i = 0; i < 32; i++) {
		CHESS_TEAM_SET* set = i % 2 == 0 ? whiteTeam : blackTeam;
		int j = i / 2;

		CHESS_PIECE piece = *(set->pieces + j);
		Instance* instance = piece.pieceInstance;
		PT_IMAGELABEL* pieceImg = (PT_IMAGELABEL*)instance->subInstance;
		PT_GUI_OBJ* pieceObj = pieceImg->guiObj;
		PT_BINDABLE_EVENT_bind(&pieceObj->e_obj_pressed, on_piece_activated);
	}

	chessGame->currentTurnTeam = whiteTeam;
	chessGame->whiteTeam = whiteTeam;
	chessGame->blackTeam = blackTeam;
	chessGame->boardFrame = boardFrame;

	currentGame = chessGame;

	return chessGame;
}

void CHESS_GAME_update(CHESS_GAME* chessGame) {
	Instance* boardFrameInstance = chessGame->boardFrame;
	PT_GUI_OBJ* boardFrame = (PT_GUI_OBJ*)boardFrameInstance->subInstance;
	vec2i boardTopLeft = boardFrame->lastAbsoluteDim.position;
	vec2i boardSize = boardFrame->lastAbsoluteDim.size;
	int cellWidth = boardSize.x / 10;
}