#pragma once
#include <windows.h>
typedef struct gameServerConfiguration GameServerConfiguration;


struct gameServerConfiguration
{
	float fVelocityBall;
	float[3] fBonusProbabilities;
	WORD wWidthGameBoard, wHeightGameBoard;	
	WORD wWidthPlayerBlock, wHeightPlayerBlock;
};