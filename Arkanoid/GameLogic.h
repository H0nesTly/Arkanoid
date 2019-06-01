#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"

#define DEFAULT_WIDTH_OF_GAMEBOARD 400
#define DEFAULT_HEIGTH_OF_GAMEBOARD 600

VOID moveBall(Game*);

VOID createLevel(Game*);

VOID createGameBoard(WORD, WORD, WORD, WORD, GameBoard*);

VOID createBall(WORD, WORD, Game*);

VOID createPlayerBlock(WORD, WORD, WORD, WORD, const PTCHAR, Game*);

VOID createBlocks(WORD, WORD, WORD, WORD, TypesOfBlock, Game*);

VOID createBonus(WORD, WORD, WORD, WORD, TypeOfBonus, Game*);

VOID destroyBonus(WORD, Game*);

VOID destroyBlock(WORD, Game*);

VOID destroyBall(WORD, Game*);

VOID destroyPlayerBlock(const PTCHAR username, WORD wNumberOfPlayers, Game* gameObj);

VOID catchBonus(WORD, Game*);

VOID incrementHealth(Game*);

BOOL decrementHealth(Game*);

//Retorna 1 caso seja detetada colisao
BOOL checkColissionBallObject(Ball*, const Coords*, const WORD, const WORD );
