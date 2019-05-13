#include "stdafx.h"
#include "DllSetup.h"
#include "GameStructures.h"
#include "MessageProtocol.h"

BOOL initComponets(ClientConnection* ccArg)
{
	switch (ccArg->typeOfConnection)
	{
	case clientSharedMemoryConnection:
		return initSharedMemory(&ccArg->SharedMem);
		break;
	case clientNamedPipeLocalConnection:
		return initNamedPipeLocal(&ccArg->PipeLocal);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
	return FALSE;
}

VOID freeComponents(ClientConnection* ccArg)
{
	switch (ccArg->typeOfConnection)
	{
	case clientSharedMemoryConnection:
		freeMappedMemory(
			ccArg->SharedMem.hGame,
			ccArg->SharedMem.lpGame,
			ccArg->SharedMem.hMessage,
			ccArg->SharedMem.lpMessage);

		freeSyncObjects(
			ccArg->SharedMem.hSemaphoreWriteMessageToServer,
			ccArg->SharedMem.hEventReadNewMessage,
			ccArg->SharedMem.hMutexWriteNewMessage);

		break;
	case clientNamedPipeLocalConnection:
		freeNamedPipeLocal(ccArg->PipeLocal);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

/*--------------- FIM Geral Init/Free ---------------*/

BOOL initSharedMemory(SharedMemory* shArg)
{
	return initClientGameMem(&shArg->hGame, &shArg->lpGame) && 
		initClientMessageMem(&shArg->hMessage, &shArg->lpMessage) &&
		initSyncObjects(&shArg->hEventReadNewMessage, &shArg->hSemaphoreWriteMessageToServer, &shArg->hMutexWriteNewMessage);
}

BOOL initClientGameMem(HANDLE* hMapObj, LPVOID* lpSharedMem)
{
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READONLY,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME
	);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}


	if (GetLastError() != ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		_tprintf(TEXT("Mapear memoria do Jogo %d \n"), GetLastError());
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,
		FILE_MAP_READ,
		0,
		0,
		0);

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL initClientMessageMem(HANDLE* hMapObj, LPVOID* lpSharedMem)
{
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MessageQueue),
		NAME_SHARED_MEMORY_MESSAGE
	);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS) // A dll foi chamada antes do servidor ERRO!!!!
	{
		_tprintf(TEXT("Mapear memoria das Mensagens %d \n"), GetLastError());
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0);

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

VOID freeMappedMemory(HANDLE hMapObjGame, LPVOID lpSharedGame, HANDLE hMapObjMessage, LPVOID lpSharedMessage)
{
	UnmapViewOfFile(lpSharedGame);
	UnmapViewOfFile(lpSharedMessage);

	CloseHandle(hMapObjGame);
	CloseHandle(hMapObjMessage);
}

BOOL initSyncObjects(HANDLE* hRObj, HANDLE* hwSemaphore, HANDLE* hMutex)
{
	*hRObj = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_SERVER_READ);

	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	*hwSemaphore = OpenSemaphore(
		SEMAPHORE_ALL_ACCESS,		//security attributes
		TRUE,		//Herança do handler
		NAME_SEMAPHORE_OBJECT_SERVER_READ);

	*hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,
		TRUE,
		NAME_MUTEX_OBJECT_CLIENT_WRITE_MESSAGE);

	_tprintf(TEXT("Criou eventos \n"));

	return *hRObj == NULL || *hwSemaphore == NULL || *hMutex == NULL ? FALSE : TRUE;
}

VOID freeSyncObjects(HANDLE hWObj, HANDLE hRObj, HANDLE hMutex)
{
	CloseHandle(hWObj);
	CloseHandle(hRObj);
	CloseHandle(hMutex);
}

BOOL initNamedPipeLocal(PipeLocal* plArg)
{
	plArg->hNamedPipe = CreateFile(
		NAME_NAMED_PIPE,	//Nome do Pipe		
		GENERIC_READ |		//Acessos
		GENERIC_WRITE,
		0,					//modo de partilha
		NULL,				//securty atributes
		OPEN_EXISTING,		//abrir o pipe
		0,					//atributos normais
		NULL);				//template file

	return plArg->hNamedPipe == NULL ||
		GetLastError() != ERROR_PIPE_BUSY ? FALSE : TRUE;
}

VOID freeNamedPipeLocal(PipeLocal* plArg)
{
	CloseHandle(plArg->hNamedPipe);
}