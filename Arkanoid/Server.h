#pragma once
#include "../Communicate/stdafx.h"
#include "..\Communicate\GameStructures.h"
#include "..\Communicate\MessageProtocol.h"

/*Mapea a Memoria que ira armazenar o jogo*/
BOOL intitServerGameMem(LPVOID , HANDLE );

/*Mapea a Memoria que ira armazenar o array de mensagens
	Em que o produtor - server e Consumidor - cliente*/
BOOL intitServerMessageMem(LPVOID , HANDLE );

/*Mapea a Memoria que ira armazenar o array de mensagens
	Em que o produtor - server e Consumidor - cliente*/
BOOL intitServerMessageMemReader(LPVOID , HANDLE );


