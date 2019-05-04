#pragma once
#include "stdafx.h"
#include "MessageProtocol.h"

#define MAX_PLAYER_INSTANCES 20

#define NAME_SHARED_MEMORY_GAME TEXT("dllSharedMemGame")

typedef struct coords Coords;
typedef struct ball Ball;
typedef struct playerBlock PlayerBlock;
typedef struct block Block; 
typedef struct bonus BonusBlock;
typedef struct player Player; 
typedef struct gameBoard GameBoard;
typedef struct game Game;

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

struct playerBlock
{
	Coords playerBlockPosition;
	WORD wWidth, wHeight;
	float fVelocity;
};

struct block
{
	Coords blockPosition;
	WORD wWidth, wHeight;
	TypesOfBlock typeOfBlock;
	TCHAR tcFigure;

	WORD wNumberOfColisions;
};

struct bonus
{
	TypeOfBonus typeOfBonus;
	Coords bonusCoords;
	TCHAR tcFigure;
	WORD wRadius;
	float fVelocityDrop;
};

struct gameBoard
{
	Coords gameBoardPosition;
	WORD wWidth, wHeight;
};

struct player
{
	WORD wLifes;
	TCHAR tcUserName[MAX_LENGTH_NAME];
	DWORD dwScore;
	PlayerBlock myPlayerBlock;
	//HANDLE hFIFO; //OPCIONAL (Rever)
};

struct game
{
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