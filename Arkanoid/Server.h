#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "..\Communicate\MessageProtocol.h"
#include "ServerStructures.h"

/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(LPVOID , HANDLE );

/*Mapea a Memoria que ira armazenar o array de mensagens
	Em que o produtor - server e Consumidor - cliente*/
BOOL intitServerMessageMemWriter(LPVOID , HANDLE );

/*Mapea a Memoria que ira armazenar o array de mensagens
	Em que o produtor - server e Consumidor - cliente*/
BOOL intitServerMessageMemReader(LPVOID , HANDLE );

void freeMappedMemory(ServerHandles* );

