#include "Server.h"

BOOL intitServerGameMem(LPVOID lpSharedMem, HANDLE hMapObj)
{
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


	//ja existe uma instancia do servidor a correr
	if (GetLastError() == ERROR_ALREADY_EXISTS)
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

BOOL intitServerMessageMemWriter(LPVOID lpSharedMem, HANDLE hMapObj)
{
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		(sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_READER_SIZE),
		NAME_SHARED_MEMORY_MESSAGE_WRITER);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
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

	ZeroMemory(lpSharedMem, sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_READER_SIZE);

	return TRUE;
}

BOOL intitServerMessageMemReader(LPVOID lpSharedMem, HANDLE hMapObj)
{
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		(sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_SIZE),
		NAME_SHARED_MEMORY_MESSAGE);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,
		FILE_MAP_READ, //write/read
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

VOID freeMappedMemory(ServerSharedMemoryHandlers* mapped)
{
	UnmapViewOfFile(mapped->lpSharedMemGame);
	UnmapViewOfFile(mapped->LpSharedMemMessage);
	UnmapViewOfFile(mapped->LpSharedMemMessageWriter);

	CloseHandle(mapped->hMapObjGame);
	CloseHandle(mapped->hMapObjMessage);
	CloseHandle(mapped->hMapObjMessageWriter);
}