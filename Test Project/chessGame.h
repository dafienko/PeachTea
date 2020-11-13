#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "chessTeam.h"
#include "PeachTea.h"

typedef struct {
	Instance* boardFrame;

	CHESS_TEAM_SET* whiteTeam;
	CHESS_TEAM_SET* blackTeam;

	CHESS_TEAM_SET* currentTurnTeam;
} CHESS_GAME;

CHESS_GAME* create_chess_game(Instance* boardFrame);

#endif