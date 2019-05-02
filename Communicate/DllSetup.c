#include "stdafx.h"
#include "DllSetup.h"
#include "GameStructures.h"
#include "MessageProtocol.h"

extern LPVOID lpgSharedMemGame;
extern HANDLE hgMapObjGame;

//Ponteiro para objetos das mensagens
extern PVOID lpgSharedMemMessage;
extern HANDLE hgMapObjMessage;

extern HANDLE hgWriteObject;
extern HANDLE hgReadObject;

BOOL initClientGameMem()
{
	hgMapObjGame = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME
	);

	if (hgMapObjGame == NULL)
	{
		return FALSE;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		return FALSE;
	}

	lpgSharedMemGame = MapViewOfFile(
		hgMapObjGame,
		FILE_MAP_READ,
		0,
		0,
		0);

	if (lpgSharedMemGame != NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL initClientMessageMem()
{
	hgMapObjMessage = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		sizeof(MessageQueue),
		NAME_SHARED_MEMORY_MESSAGE
	);

	if (hgMapObjMessage == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		return FALSE;
	}

	lpgSharedMemMessage = MapViewOfFile(
		hgMapObjMessage,
		FILE_MAP_READ,
		0,
		0,
		0);

	if (lpgSharedMemMessage != NULL)
	{
		return FALSE;
	}

	return TRUE;
}

VOID freeMappedMemory()
{
	UnmapViewOfFile(lpgSharedMemGame);
	UnmapViewOfFile(lpgSharedMemMessage);

	CloseHandle(hgMapObjGame);
	CloseHandle(hgMapObjMessage);
}

BOOL initSyncObjects()
{
	hgReadObject = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NAME_EVENT_OBJECT_SERVER_READ);

	hgWriteObject = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NAME_EVENT_OBJECT_SERVER_WRITE);

	return hgReadObject == NULL || hgWriteObject == NULL ? FALSE : TRUE;
}

VOID freeSyncObjects()
{
	CloseHandle(hgWriteObject);
	CloseHandle(hgReadObject);
}