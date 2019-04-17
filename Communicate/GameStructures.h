#pragma once
#include "stdafx.h"

#define MAX_PLAYER_INSTANCES 20 //MODIFICAR

typedef struct coords Coords;
typedef struct ball Ball;
typedef struct playerBlock PlayerBlock;
typedef struct player Player; 
typedef struct gameBoard GameBoard;
typedef struct game Game;

typedef enum typesOfBlocks TypesOfBlock;

struct coords
{
	UINT x, y;
};

struct ball
{
	Coords ballPosition;
	WORD radius;
	float fVelocity;
};

struct playerBlock
{
	Coords playerBlockPosition;
	WORD wWidth, wHeight;
	//float fVelocity;
};

struct block
{
	Coords blockPosition;
	WORD wWidth, wHeight;
	TypesOfBlock typeOfBlock;
	WORD wNumberOfColisions;
};

struct gameBoard
{
	Coords gameBoardPosition;
	WORD wWidth, wHeight;
};

struct player
{
	WORD wLifes;
	TCHAR[MAX_LENGTH_NAME] tcUserName;
	DWORD dwScore;
	PlayerBlock myPlayerBlock;
	HANDLE hFIFO; //OPCIONAL
};

struct game
{
	GameBoard myGameBoard;
	Player[MAX_PLAYER_INSTANCES] playesConnected;
};

enum typesOfBlocks
{
	Normal,
	Magic,
	Rigid
};