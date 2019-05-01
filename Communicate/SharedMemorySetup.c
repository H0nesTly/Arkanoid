#include "stdafx.h"
#include "SharedMemorySetup.h"
#include "GameStructures.h"
#include "MessageProtocol.h"


BOOL initClientGameMem(LPVOID lpSharedMem, HANDLE hMapObj)
{
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME
	);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,
		FILE_MAP_READ,
		0,
		0,
		0);

	if (lpSharedMem != NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL initClientMessageMemReader(LPVOID lpSharedMem, HANDLE hMapObj)
{
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		(sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_READER_SIZE),
		NAME_SHARED_MEMORY_MESSAGE
	);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,
		FILE_MAP_READ,
		0,
		0,
		0);

	if (lpSharedMem != NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL initClientMessageMemWriter(LPVOID lpSharedMem, HANDLE hMapObj)
{
	hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		(sizeof(MessageProtocolDatagram) * MESSAGE_QUEUE_SIZE),
		NAME_SHARED_MEMORY_MESSAGE_WRITER
	);

	if (hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		return FALSE;
	}

	lpSharedMem = MapViewOfFile(
		hMapObj,
		FILE_MAP_WRITE,
		0,
		0,
		0);

	if (lpSharedMem != NULL)
	{
		return FALSE;
	}

	return TRUE;

}
