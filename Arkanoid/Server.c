#include "Server.h"


BOOL intitServerGameMem(HANDLE* hMapObj, LPVOID* lpSharedMem )
{
	//Mapear sharedMem para o jogo
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE, //usar paging file
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}


	//ja existe uma instancia do servidor a correr
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,					// qual o objeto a mapear
		FILE_MAP_WRITE,				//read/write 
		0,			//HIGH bit
		0,				//Low bit
		0);		//inicio da mem

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}

	//Limpar memoria
	ZeroMemory(*lpSharedMem, sizeof(Game));

	return TRUE;
}

BOOL intitServerMessageMem(HANDLE* hMapObj, LPVOID* lpSharedMem)
{
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MessageQueue),
		NAME_SHARED_MEMORY_MESSAGE);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,
		FILE_MAP_ALL_ACCESS, //write/read
		0,
		0,
		0);

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}

	ZeroMemory(*lpSharedMem, sizeof(MessageQueue));

	return TRUE;
}

VOID freeMappedMemory(ServerSharedMemoryHandlers* mapped)
{
	UnmapViewOfFile(mapped->lpSharedMemGame);
	UnmapViewOfFile(mapped->LpSharedMemMessage);

	CloseHandle(mapped->hMapObjGame);
	CloseHandle(mapped->hMapObjMessage);
}

static BOOL checkUserNameInLobby(PTCHAR userName, const ServerGameInstance* gameArg)
{
	//Vamos ver se nome no lobby
	if (gameArg->GameStates == WaitingForPlayers)
	{
		for (size_t i = 0; i < gameArg->lobbyGame.wPlayersInLobby; i++)
		{
			if (_tcscmp(gameArg->lobbyGame.playersInLobby[i].tcUserName, userName) == 0)
			{
				return FALSE;
			}
		}

	}
	return TRUE;
}

//TODO: VER A MEMORIA PARTILHADA SE ALGUM PLAYER EM JOGO
BOOL addUserNameToLobby(PTCHAR userName, ServerGameInstance* gameLobby)
{
	if (checkUserNameInLobby(userName, gameLobby))
	{
		_tcscpy_s(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby++].tcUserName,	//destino
			_countof(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby].tcUserName),	//tamanho que o destino suporta
			userName		//Origem
		);

		return TRUE;
	}
	return FALSE;
}