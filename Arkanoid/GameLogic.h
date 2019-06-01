#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"

#define DEFAULT_WIDTH_OF_GAMEBOARD 400
#define DEFAULT_HEIGTH_OF_GAMEBOARD 600

#define HEIGTH_OF_PLAYER_PADDLE 15

#define DEFAULT_POS_Y_PLAYER_PADDLE  (DEFAULT_HEIGTH_OF_GAMEBOARD - 100)
#define DEFAULT_HEIGTH_LOSE_BALL  (DEFAULT_POS_Y_PLAYER_PADDLE + HEIGTH_OF_PLAYER_PADDLE + 1)


VOID moveBall(Game*);

VOID createLevel(Game*);

VOID createGameBoard(WORD, WORD, WORD, WORD, GameBoard*);

VOID createBall(WORD, WORD, Game*);

VOID createPlayerPaddle(WORD, WORD, WORD, WORD, const PTCHAR, Game*);

VOID createBlocks(WORD, WORD, WORD, WORD, TypesOfBlock, Game*);

VOID createBonus(WORD, WORD, WORD, WORD, TypeOfBonus, Game*);

VOID destroyBonus(WORD, Game*);

VOID destroyBlock(WORD, Game*);

VOID destroyBall(WORD, Game*);

VOID destroyPlayerPaddle(const PTCHAR , Game* );

VOID catchBonus(WORD, Game*);

VOID incrementHealth(Game*);

BOOL decrementHealth(Game*);

//Retorna 1 caso seja detetada colisao
BOOL checkColissionBallObject(Ball*, const Coords*, const WORD, const WORD );
