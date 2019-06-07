#pragma once
#include "stdafx.h"


#define DEFAULT_WIDTH_OF_GAMEBOARD 400
#define DEFAULT_HEIGTH_OF_GAMEBOARD 600

#define NORMAL_SPACING 1

#define HEIGTH_OF_PLAYER_PADDLE 15

#define DEFAULT_WIDTH_OF_PLAYER_PADDLE 50

#define DEFAULT_POS_Y_PLAYER_PADDLE  (DEFAULT_HEIGTH_OF_GAMEBOARD - 125)
#define DEFAULT_HEIGTH_LOSE_BALL  (DEFAULT_POS_Y_PLAYER_PADDLE + (HEIGTH_OF_PLAYER_PADDLE * 3) + NORMAL_SPACING)

#define MAX_PLAYER_INSTANCES 20

#define NUM_OF_OBJ_GAME 20
#define NUM_OF_BLOCK_OBJ_GAME 40
#define DEFAULT_BALL_UNITS_TO_MOVE 4
#define DEFAULT_PADDLE_VELOCITY 4
#define DEFAULT_HEALTH 3

#define DEFAULT_BALL_VELOCITY 10

#define NAME_SHARED_MEMORY_GAME TEXT("dllSharedMemGame")

typedef struct coords Coords;
typedef struct ball Ball;
typedef struct paddle Paddle;
typedef struct block Block;
typedef struct bonus BonusBlock;
typedef struct player Player;
typedef struct scorePlayer ScorePlayer;
typedef struct gameBoard GameBoard;
typedef struct game Game;
typedef struct playerInfo PlayerInfo;

typedef enum typesOfBlocks TypesOfBlock;
typedef enum typeOfBonus TypeOfBonus;

struct coords
{
	WORD x, y;
};

struct ball
{
	Coords ballPosition;
	TCHAR tcFigure;

	WORD wWitdh;
	WORD wHeight;

	int nMovementVectorX, nMovementVectorY;

	WORD wVelocity;
	WORD wUnitsToMove;
};

struct playerInfo
{
	TCHAR tcUserName[MAX_LENGTH_NAME];
	union
	{
		HANDLE hNamedPipe;
		HANDLE hEventSharedMemory;
	}typeOfConnection;
};

struct player
{
	PlayerInfo playerInfo;
};

struct paddle
{
	Coords playerBlockPosition;
	Player playerOwnerOfBlock;
	WORD wWidth, wHeight;
	WORD wUnitsToMove;
};

struct block
{
	Coords blockPosition;
	WORD wWidth, wHeight;

	TypesOfBlock typeOfBlock;

	WORD wNumberOfColisions;
};

struct bonus
{
	Coords bonusCoords;
	WORD wWidth, wHeight;

	TypeOfBonus typeOfBonus;
	WORD wDropUnits;
};

struct gameBoard
{
	Coords gameBoardPosition;
	WORD wWidth, wHeight;
};


struct game
{
	WORD wNumberOfBonusDropping;
	BonusBlock bonusBlock[NUM_OF_OBJ_GAME];

	WORD wNumberOfBlocks;
	Block blocks[NUM_OF_OBJ_GAME * 2];

	WORD wNumberOfPlayerPaddles;
	Paddle PlayerPaddles[NUM_OF_OBJ_GAME];

	WORD wNumberOfBalls;
	Ball ball[NUM_OF_OBJ_GAME];

	WORD wLifes;
	DWORD dwScore;
	GameBoard myGameBoard;
};

enum typesOfBlocks
{
	Normal,
	Magic,
	Rigid
};

enum typeOfBonus
{
	None = 0,
	SpeedUp,
	SlowDown,
	ExtraHealth,
	Triple
};

