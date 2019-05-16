#include "stdafx.h"
#include "DllSetup.h"
#include "GameStructures.h"
#include "MessageProtocol.h"

BOOL initComponetsDLL(ClientConnection* ccArg)
{
	switch (ccArg->typeOfConnection)
	{
	case clientSharedMemoryConnection:
		return initSharedMemoryDLL(&ccArg->SharedMem);
		break;
	case clientNamedPipeLocalConnection:
		return initNamedPipeLocalDLL(&ccArg->PipeLocal);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
	return FALSE;
}

VOID freeComponentsDLL(ClientConnection* ccArg)
{
	switch (ccArg->typeOfConnection)
	{
	case clientSharedMemoryConnection:
		freeMappedMemoryDLL(
			ccArg->SharedMem.hGame,
			ccArg->SharedMem.lpGame,
			ccArg->SharedMem.hMessage,
			ccArg->SharedMem.lpMessage);

		freeSyncObjectsDLL(
			ccArg->SharedMem.hSemaphoreWriteMessageToServer,
			ccArg->SharedMem.hEventReadNewMessage,
			ccArg->SharedMem.hMutexWriteNewMessage);

		break;
	case clientNamedPipeLocalConnection:
		freeNamedPipeLocalDLL(&ccArg->PipeLocal);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

/*--------------- FIM Geral Init/Free ---------------*/

BOOL initSharedMemoryDLL(SharedMemory* shArg)
{
	return initClientGameMemDLL(&shArg->hGame, &shArg->lpGame) && 
		initClientMessageMemDLL(&shArg->hMessage, &shArg->lpMessage) &&
		initSyncObjectsDLL(&shArg->hEventReadNewMessage, &shArg->hSemaphoreWriteMessageToServer, &shArg->hMutexWriteNewMessage);
}

BOOL initClientGameMemDLL(HANDLE* hMapObj, LPVOID* lpSharedMem)
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

BOOL initClientMessageMemDLL(HANDLE* hMapObj, LPVOID* lpSharedMem)
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

VOID freeMappedMemoryDLL(HANDLE hMapObjGame, LPVOID lpSharedGame, HANDLE hMapObjMessage, LPVOID lpSharedMessage)
{
	UnmapViewOfFile(lpSharedGame);
	UnmapViewOfFile(lpSharedMessage);

	CloseHandle(hMapObjGame);
	CloseHandle(hMapObjMessage);
}

BOOL initSyncObjectsDLL(HANDLE* hRObj, HANDLE* hwSemaphore, HANDLE* hMutex)
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

VOID freeSyncObjectsDLL(HANDLE hWObj, HANDLE hRObj, HANDLE hMutex)
{
	CloseHandle(hWObj);
	CloseHandle(hRObj);
	CloseHandle(hMutex);
}

BOOL initNamedPipeLocalDLL(PipeLocal* plArg)
{
	BOOL fReturn;
	DWORD dwPipeMode
	plArg->hNamedPipe = CreateFile(
		NAME_NAMED_PIPE,	//Nome do Pipe		
		GENERIC_READ |		//Acessos
		GENERIC_WRITE,
		0,					//modo de partilha
		NULL,				//securty atributes
		OPEN_EXISTING,		//abrir o pipe
		FILE_ATTRIBUTE_NORMAL,			//atributos normais
		NULL);				//template file

	 if (plArg->hNamedPipe == INVALID_HANDLE_VALUE ||
		GetLastError() != ERROR_PIPE_BUSY)
	 {
		return  FALSE; 
	 }
	 // The pipe connected; change to message-read mode. 
	 /*The client side of a named pipe starts out in byte mode, even if the server side is in message mode.
	  * To avoid problems receiving data, set the client side to message mode as well. 
	  * To change the mode of the pipe, the pipe client must open a read-only pipe with 
	  * GENERIC_READ and FILE_WRITE_ATTRIBUTES access.
	  */

	dwPipeMode = PIPE_READMODE_MESSAGE;
	fReturn = SetNamedPipeHandleState(
		plArg->hNamedPipe,	//handler
		 &dwPipeMode,		//Modo do pipe
		 NULL,				//Tamnanho em bytes 
		 NULL				//timeout
	 );

	return fReturn ? TRUE : FALSE;

}

VOID freeNamedPipeLocalDLL(PipeLocal* plArg)
{
	CloseHandle(plArg->hNamedPipe);
}