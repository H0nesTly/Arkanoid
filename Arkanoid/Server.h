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

/*@return - true se não existir username ja no servidor*/
BOOL checkUserNameInLobby(PTCHAR, const ServerGameInstance *);

BOOL addUserNameToLobby(PTCHAR, ServerGameInstance*);
