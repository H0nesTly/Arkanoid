#include "Server.h"

BOOL intitServerGameMem(LPVOID lpSharedMem, HANDLE hMapObj)
{
	BOOL fInit;
	//Mapear sharedMem para o jogo
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE, //usar paging file
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	fInit = (GetLastError() != ERROR_ALREADY_EXISTS);

	//ja existe uma instancia do servidor a correr
	if (fInit)
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,					// qual o objeto a mapear
		FILE_MAP_WRITE,				//read/write 
		0,			//HIGH bit
		0,				//Low bit
		0);		//inicio da mem

	if (lpSharedMem == NULL)
	{
		return FALSE;
	}

	//Limpar memoria
	ZeroMemory(lpSharedMem, sizeof(Game));

	return TRUE;
}

BOOL intitServerMessageMem(LPVOID lpSharedMem, HANDLE hMapObj)
{
	BOOL fInit;

	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		(sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_SIZE),
		NAME_SHARED_MEMORY_MESSAGE);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	fInit = (GetLastError() != ERROR_ALREADY_EXISTS);

	if (!fInit)
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,
		FILE_MAP_WRITE, //write/read
		0,
		0,
		0);

	if (lpSharedMem == NULL)
	{
		return FALSE;
	}

	ZeroMemory(lpSharedMem, sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_SIZE);

	return TRUE;
}