#pragma once
#include "..\Communicate\stdafx.h"
#include "ServerStructures.h"
#include "GameLogic.h"

#define TEXT_FILE_LEVELS TEXT("niveis")
#define TEXT_FILE_INITIAL_LIFES TEXT("vidasiniciais")
#define TEXT_TIME_TO_WAIT_FOR_PLAYERS TEXT("esperaPorJogadores")
#define TEXT_FILE_DURATION_BONUS TEXT("duracaoEfeitos")
#define TEXT_FILE_PROB_SPEED_UPS TEXT("probabilidadespeedup")
#define TEXT_FILE_PROB_SLOW_DOWNS TEXT("probabilidadeslowdown")
#define TEXT_FILE_PROB_EXTRA_HEALTH TEXT("probabilidadevidaextra")
#define TEXT_FILE_PROB_BONUS_TRIPLE TEXT("probabilidadeTriple")
#define TEXT_FILE_BALL_SPEED TEXT("velocidadebola")

#define TEXT_FILE_INITIAL_BLOCKS TEXT("tijolosiniciais")

#define TEXT_REGISTY_PATH TEXT("Software\\Arkanoid")

extern GameServerConfiguration serverConfig;

/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(HANDLE*, LPVOID*);

/*Mapea a Memoria que ira armazenar o array de mensagens*/
BOOL intitServerMessageMem(HANDLE*, LPVOID*);

BOOL initServerPipe(NamedPipeInstance[], WORD);

void freeMappedMemoryServer(ServerSharedMemoryHandlers*);

BOOL loadGameConfiguration(TCHAR *nomeFicheiro, GameServerConfiguration *serverConfig);

BOOL setTopTenRegistry(ScorePlayer scoreTopTen[]);

BOOL getTopTenRegistry(ScorePlayer scoreTopTen[]);

VOID setScoreTopTen(ScorePlayer newScore, ScorePlayer scoreTopTen[]);

BOOL verifyUserName(PTCHAR);

VOID broadCastGameData(NamedPipeInstance*, Server*);

VOID writeMessageToClientSharedMemory(MessageQueue*, TypeOfResponseMessage, const PTCHAR, const PTCHAR);

VOID writeMessageToClientPipeResponse(MessageProtocolPipe*, TypeOfResponseMessage, const PTCHAR, const PTCHAR);

/*@return - true se não existir username ja no servidor*/
BOOL checkUserNameInLobby(PTCHAR, const ServerGameInstance *);

BOOL addUserNameToLobby(PTCHAR, Server*);

WORD getPlayersInLobby(const Lobby*);

VOID addUsersToGame(Server*);

VOID transferPlayersToGame(Server*);

VOID startGame(Server*);

VOID removePlayer(Server*, const PTCHAR);

BOOL removePlayerFromLobby(Lobby*, const PTCHAR);

BOOL removePlayerFromGame(UsersPlaying*, Game*, const PTCHAR);

VOID tryToMovePaddle(const PTCHAR, Server*, const short);
/*--------------- NAMED PIPES ---------------*/
VOID initSecurityAtributes(SECURITY_ATTRIBUTES*);

BOOL ConnectNewClientToNP(HANDLE, LPOVERLAPPED);

VOID DisconnectAndReconnect(NamedPipeInstance*);
