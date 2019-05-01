#pragma once
#include "ServerThreads.h"
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"

typedef struct serverHandles ServerHandlers;
typedef struct serverThreadsHandlers ServerThreadsHandlers;
typedef struct serverSharedMemoryHandlers ServerSharedMemoryHandlers;
typedef struct gameServerConfiguration GameServerConfiguration;
typedef struct lobby Lobby;
typedef struct serverGameInstance ServerGameInstance;

typedef enum stateOfGame StateOfGame;


struct serverSharedMemoryHandlers
{
	LPVOID	lpSharedMemGame;
	LPVOID	LpSharedMemMessage;
	LPVOID	LpSharedMemMessageWriter;
	HANDLE	hMapObjGame;
	HANDLE	hMapObjMessage;
	HANDLE	hMapObjMessageWriter;
};

struct serverHandles
{
	ServerSharedMemoryHandlers sharedMemHandlers;
	ServerThreadsHandlers threadHandlers;
};

struct gameServerConfiguration
{
	float fVelocityBall;
	float fBonusProbabilities[3];
	WORD wWidthGameBoard, wHeightGameBoard;	
	WORD wWidthPlayerBlock, wHeightPlayerBlock;
};

struct lobby
{
	TCHAR playersInLobby[MAX_PLAYER_INSTANCES];
	WORD wPlayersInLobby;
};

struct gameServerGameInstance
{
	//jogadores a jogar/a ver
	Lobby lobbyGame;
	GameServerConfiguration GameConfiguration;
	StateOfGame GameStates;
};

enum stateOfGame
{
	WaitForPlayers,
	GameInProgress,
	GameOver
};