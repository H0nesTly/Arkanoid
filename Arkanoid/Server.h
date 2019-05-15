#pragma once
#include "ServerStructures.h"

#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "..\Communicate\MessageProtocol.h"

#define TEXT_FILE_LEVELS TEXT("niveis")
#define TEXT_FILE_SPEED_UPS TEXT("speedups")
#define TEXT_FILE_SLOW_DOWNS TEXT("slowdowns")
#define TEXT_FILE_INITIAL_LIFES TEXT("vidasiniciais")
#define TEXT_FILE_INITIAL_BLOCKS TEXT("tijolosiniciais")
#define TEXT_FILE_PROB_SPEED_UPS TEXT("probabilidadespeedup")
#define TEXT_FILE_PROB_SLOW_DOWNS TEXT("probabilidadeslowdown")
#define TEXT_FILE_PROB_BONUS TEXT("probabilidadebonus")
#define TEXT_FILE_DURATION_LEVEL TEXT("duracao")
#define TEXT_FILE_BALL_SPEED TEXT("velocidadebola")

#define TEXT_REGISTY_PATH TEXT("Software\\Arkanoid")

/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(HANDLE*, LPVOID*);

/*Mapea a Memoria que ira armazenar o array de mensagens*/
BOOL intitServerMessageMem(HANDLE*, LPVOID*);

BOOL initServerPipeLocal(NamedPipeInstance*, HANDLE* ,WORD);

void freeMappedMemoryServer(ServerSharedMemoryHandlers* );

BOOL loadGameConfiguration(TCHAR *nomeFicheiro, GameServerConfiguration *serverConfig);

BOOL setTopTenRegistry(ScorePlayer scoreTopTen[]);

BOOL getTopTenRegistry(ScorePlayer scoreTopTen[]);

VOID setScoreTopTen(ScorePlayer newScore, ScorePlayer scoreTopTen[]);

BOOL verifyUserName(PTCHAR);

/*@return - true se não existir username ja no servidor*/
BOOL checkUserNameInLobby(PTCHAR, const ServerGameInstance *);

BOOL addUserNameToLobby(PTCHAR, ServerGameInstance*);

/*TODO: INCREMENTAR MENSAGENS POR LER*/
VOID writeMessageToClient(MessageQueue*, TypeOfResponseMessage, const PTCHAR , const PTCHAR);

/*--------------- NAMED PIPES ---------------*/
BOOL waitNewClientNP(HANDLE, LPOVERLAPPED);
