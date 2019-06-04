#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "Server.h"

typedef struct gameServerConfiguration GameServerConfiguration;
typedef struct coords Coords;
typedef struct ball Ball;
typedef struct paddle Paddle;
typedef struct block Block;
typedef struct bonus BonusBlock;
typedef struct game Game;

VOID moveBall(Game*);

VOID moveBonus(Game*, WORD);

VOID movePaddle(const PTCHAR, Game*, const short);

VOID createLevel(Game*);

VOID createGameBoard(WORD, WORD, WORD, WORD, GameBoard*);

VOID createBall(WORD, WORD, Game*);

VOID createPlayerPaddle(const PTCHAR, Game*);

VOID createBlocks(WORD, WORD, WORD, WORD, TypesOfBlock, Game*);

VOID createBonus(WORD, WORD, WORD, WORD, TypeOfBonus, Game*);

VOID destroyBonus(WORD, Game*);

VOID destroyBlock(WORD, Game*);

VOID destroyBall(WORD, Game*);

VOID destroyPlayerPaddle(const PTCHAR, Game*);

VOID catchBonus(WORD, Game*);

VOID incrementHealth(Game*);

BOOL decrementHealth(Game*);

WORD generateWidthOfPlayePaddle(const Game*);

//Retorna 1 caso seja detetada colisao
BOOL checkColissionBallObject(Ball*, const Coords*, const WORD, const WORD);

BOOL checkColissionBonusObject(const BonusBlock*, const Coords*, const WORD, const WORD);

int getPaddleOwnerByName(const PTCHAR, Game*);

