#pragma once
#include "stdafx.h"
#include "MessageProtocol.h"

#define MAX_PLAYER_INSTANCES 20

#define NUM_OF_OBJ_GAME 20

#define NAME_SHARED_MEMORY_GAME TEXT("dllSharedMemGame")

typedef struct coords Coords;
typedef struct ball Ball;
typedef struct playerBlock PlayerBlock;
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
	float fVelocity;
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

struct playerBlock
{
	Coords playerBlockPosition;
	Player playerOwnerOfBlock;
	WORD wWidth, wHeight;
	float fVelocity;
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
	Block blocks[NUM_OF_OBJ_GAME];
	PlayerBlock playerBlocks[NUM_OF_OBJ_GAME];
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
	SpeedUp,
	SlowDown,
	ExtraHealth,
	Triple
};

