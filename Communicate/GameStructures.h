#pragma once
#include "stdafx.h"
#include "MessageProtocol.h"

#define MAX_PLAYER_INSTANCES 20

#define NUM_OF_OBJ_GAME 20
#define NUM_OF_BLOCK_OBJ_GAME 40
#define DEFAULT_BALL_VELOCITY 4
#define DEFAULT_PADDLE_VELOCITY 10

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
	WORD wRadius;

	WORD wWitdh;
	WORD wHeigth;

	int nMovementVectorX, nMovementVectorY;

	WORD wVelocity;
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
	DWORD dwScore;
};

struct paddle
{
	Coords playerBlockPosition;
	Player playerOwnerOfBlock;
	WORD wWidth, wHeight;
	WORD wVelocity;
};

struct block
{
	Coords blockPosition;
	WORD wWidth, wHeight;
	TypesOfBlock typeOfBlock;
	TCHAR tcFigure;//retirar

	WORD wNumberOfColisions;
};

struct bonus
{
	TypeOfBonus typeOfBonus;
	Coords bonusCoords;
	TCHAR tcFigure; // retirar
	WORD wRadius;
	float fVelocityDrop;
};

struct gameBoard
{
	Coords gameBoardPosition;
	WORD wWidth, wHeight;
};


struct game
{
	BonusBlock bonusBlock[NUM_OF_OBJ_GAME];

	WORD wNumberOfBlocks;
	Block blocks[NUM_OF_OBJ_GAME * 2];

	WORD wNumberOfPlayerPaddles;
	Paddle PlayerPaddles[NUM_OF_OBJ_GAME];
	Ball ball;

	WORD wLifes;
	GameBoard myGameBoard;

	Player playersConnected[MAX_PLAYER_INSTANCES];
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

