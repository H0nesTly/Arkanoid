#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"
#include "..\Communicate\GameStructures.h"

#include "Server.h"
#include "ServerThreads.h"
#include "ServerSyncObj.h"
#include "ServerThreads.h"

typedef struct server Server;
typedef struct serverGameInstance ServerGameInstance;
typedef struct serverSharedMemoryHandlers ServerSharedMemoryHandlers;
typedef struct namedPipeInstance NamedPipeInstance;
typedef struct serverHandles ServerHandlers;
typedef struct serverThreadsHandlers ServerThreadsHandlers;
typedef struct gameServerConfiguration GameServerConfiguration;
typedef struct lobby Lobby;
typedef struct usersPlaying UsersPlaying;

typedef struct playerInfo PlayerInfo;

typedef enum stateOfNamedPipe StateOfNamedPipe;
typedef enum stateOfGame StateOfGame;


struct serverSharedMemoryHandlers
{
	LPVOID	lpSharedMemGame;
	LPVOID	LpSharedMemMessage;
	HANDLE	hMapObjGame;
	HANDLE	hMapObjMessage;
};

struct namedPipeInstance
{
	OVERLAPPED oOverLapReader;
	//OVERLAPPED oOverLapWriter;
	HANDLE hNamedPipeWriteToClient;
	HANDLE hNamedPipeReadFromClient;
	HANDLE hMyEvent;
	BOOL fPendigIO;
	StateOfNamedPipe State;
	MessageProtocolPipe message;
	DWORD dwNumberOfBytesRead;
	DWORD dwNumberOfBytesWritten;
};

struct serverHandles
{
	//TODO:  Apenas Alocar FIFOS/eventos Necessários
	NamedPipeInstance namedPipeInstances[MAX_PLAYER_INSTANCES];

	ServerSharedMemoryHandlers sharedMemHandlers;
	ServerThreadsHandlers threadHandlers;
};

struct gameServerConfiguration
{
	WORD niveis;
	WORD vidasIniciais;
	WORD duracaoEfeitos;
	double probSpeedUp;
	double probSlowDowns;
	double probVidaExtra;
	double probTriple;
	WORD wVelocityBall;
	WORD wTimeToWaitForNewPlayers;
};

struct lobby
{
	PlayerInfo playersInLobby[MAX_PLAYER_INSTANCES];
	WORD wPlayersInLobby;
};


struct usersPlaying
{
	PlayerInfo playersPlaying[MAX_PLAYER_INSTANCES];
	WORD wPlayersPlaying;
};

struct serverGameInstance
{
	//jogadores a jogar/a ver
	Lobby lobbyGame;
	GameServerConfiguration GameConfiguration;
	UsersPlaying usersInGame;
	StateOfGame GameStates;
};

struct server
{
	ServerHandlers serverHandlers;
	ServerGameInstance gameInstance;
};

enum stateOfNamedPipe
{
	ConnectingState,
	ReadState,
	WriteState
};

enum stateOfGame
{
	WaitingForPlayers,
	GameInProgress,
	GameOver
};