#include "chessGame.h"
#include "chessSprites.h"

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

CHESS_GAME create_chess_game(Instance* boardFrame) {
	CHESS_GAME chessGame = { 0 };

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

	CHESS_TEAM_SET* blackTeam = create_chess_team_set(
		blackKingPos,
		blackDir,
		boardFrame,
		blackChessSprites
	);

	return chessGame;
}