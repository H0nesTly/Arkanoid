#pragma once
#include <windows.h>

typedef struct gameServerConfiguration GameServerConfiguration;
typedef struct lobby Lobby;

typedef enum stateOfGame StateOfGame;


struct gameServerConfiguration
{
	float fVelocityBall;
	float fBonusProbabilities[3];
	WORD wWidthGameBoard, wHeightGameBoard;	
	WORD wWidthPlayerBlock, wHeightPlayerBlock;
};

struct lobby
{
	TCHAR playersInLobby[MAX_PLAYER_INSTANCE];
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