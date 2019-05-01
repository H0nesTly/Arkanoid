#pragma once
#include "ServerStructures.h"
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "..\Communicate\MessageProtocol.h"


/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(LPVOID , HANDLE );

/*Mapea a Memoria que ira armazenar o array de mensagens*/
BOOL intitServerMessageMem(LPVOID , HANDLE );

void freeMappedMemory(ServerSharedMemoryHandlers* );

void createEvents();

