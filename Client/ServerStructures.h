#pragma once
#include <windows.h>

typedef struct gameServerConfiguration GameServerConfiguration;

typedef enum gameStates GameStates;


struct gameServerConfiguration
{
	float fVelocityBall;
	float fBonusProbabilities[3];
	WORD wWidthGameBoard, wHeightGameBoard;	
	WORD wWidthPlayerBlock, wHeightPlayerBlock;
};

struct gameServerGameInstance
{
	//jogadores a jogar/a ver
};

enum gameStates
{
	StartGame,
	GameInProgress,
	GameOver
};