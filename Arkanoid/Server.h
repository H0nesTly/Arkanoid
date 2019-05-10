#pragma once
#include "ServerStructures.h"

#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "..\Communicate\MessageProtocol.h"

/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(HANDLE*, LPVOID*);

/*Mapea a Memoria que ira armazenar o array de mensagens*/
BOOL intitServerMessageMem(HANDLE*, LPVOID*);

void freeMappedMemory(ServerSharedMemoryHandlers* );

BOOL leituraFicheiroConfig(TCHAR *nomeFicheiro, GameServerConfiguration *serverConfig);

BOOL setTopTenRegistry(ScorePlayer scoreTopTen[]);

BOOL getTopTenRegistry(ScorePlayer scoreTopTen[]);

VOID setScoreTopTen(ScorePlayer newScore, ScorePlayer scoreTopTen[]);

BOOL verifyUserName(PTCHAR);

/*@return - true se n�o existir username ja no servidor*/
BOOL checkUserNameInLobby(PTCHAR, const ServerGameInstance *);

BOOL addUserNameToLobby(PTCHAR, ServerGameInstance*);

/*TODO: INCREMENTAR MENSAGENS POR LER*/
VOID writeMessageToClient(MessageQueue*, TypeOfResponseMessage, PlayerInfo, PlayerInfo);
