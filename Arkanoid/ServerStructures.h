#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "Server.h"
#include "ServerThreads.h"
#include "ServerSyncObj.h"
#include "ServerThreads.h"

typedef struct server Server;
typedef struct serverGameInstance ServerGameInstance;
typedef struct serverSharedMemoryHandlers ServerSharedMemoryHandlers;
typedef struct serverHandles ServerHandlers;
typedef struct serverThreadsHandlers ServerThreadsHandlers;
typedef struct gameServerConfiguration GameServerConfiguration;
typedef struct lobby Lobby;

typedef enum stateOfGame StateOfGame;


struct serverSharedMemoryHandlers
{
	LPVOID	lpSharedMemGame;
	LPVOID	LpSharedMemMessage;
	HANDLE	hMapObjGame;
	HANDLE	hMapObjMessage;
};

struct serverHandles
{
	ServerSharedMemoryHandlers sharedMemHandlers;
	ServerThreadsHandlers threadHandlers;
};

struct gameServerConfiguration
{
	int niveis;
	int speedUps;
	int slowDowns;
	int duracao;
	double probSpeedUp;
	double probSlowDowns;
	int vidasIniciais;
	int tejolosIniciais;
	double fVelocityBall;
	double fBonusProbabilities;
	WORD wWidthGameBoard, wHeightGameBoard;	
	WORD wWidthPlayerBlock, wHeightPlayerBlock;
};

struct lobby
{
	TCHAR playersInLobby[MAX_PLAYER_INSTANCES];
	WORD wPlayersInLobby;
};

struct serverGameInstance
{
	//jogadores a jogar/a ver
	Lobby lobbyGame;
	GameServerConfiguration GameConfiguration;
	StateOfGame GameStates;
};

struct server
{
	ServerHandlers serverHandlers;
	ServerGameInstance gameInstance;
};


enum stateOfGame
{
	WaitForPlayers,
	GameInProgress,
	GameOver
};