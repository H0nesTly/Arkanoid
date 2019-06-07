#pragma once
#include "stdafx.h"
#include "GameStructures.h"

typedef struct clientConnection ClientConnection;
typedef struct windowhandlers Windowhandlers;
typedef struct sharedMemory SharedMemory;
typedef struct pipeLocal PipeLocal;
typedef struct pipeRemote PipeRemote;

typedef enum typeOfClientConnection TypeOfClientConnection;

struct clientConnection
{
	struct windowhandlers
	{
		HWND hWndMain;
		HDC memDC;
	} wndHandlers;

	TypeOfClientConnection typeOfConnection;
	TCHAR tcUserName[MAX_LENGTH_NAME];
	union
	{
		struct sharedMemory
		{
			BOOL bAlreadyAuthenticate;
			//Ponteiros e handles para Memoria virtula
			LPVOID lpMessage;
			HANDLE hMessage;
			LPVOID lpGame;
			HANDLE hGame;
			/*---Eventos e mecanismos de sincroniza��o---*/
			/*Semaforo para notificar que existe uma nova mensagem para o servidor ler*/
			HANDLE hSemaphoreWriteMessageToServer;

			HANDLE hSemaphoreReadMessageFromServer;
			/*Evento para notificar aos clientes que existe uma nova mensagem*/
			HANDLE hEventReadNewMessage;
			/*Mutex's para a escrita de novas mensagens*/
			HANDLE hMutexWriteNewMessage;

			HANDLE hEventWaitForGameData;

		} SharedMem;

		struct pipeLocal
		{
			HANDLE hNamedPipeWriteToServer;
			HANDLE hNamedPipeReadFromServer;
			Game* gameObj;
			//Estado?
		} PipeLocal;

		struct pipeRemote
		{
			int y;
		} PipeRemote;
	};
};

enum typeOfClientConnection
{
	clientSharedMemoryConnection,
	clientNamedPipeLocalConnection,
	clientNamedPipeRemoteConnection
};


BOOL initComponetsDLL(ClientConnection*);

VOID freeComponentsDLL(ClientConnection*);
/*---------- Fun�oes de Bootstrapping para memoria partilhada ----------*/

BOOL initSharedMemoryDLL(SharedMemory*);

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMemDLL(HANDLE*, LPVOID*);

/*Cria handle p da memoria mapeada*/
BOOL initClientMessageMemDLL(HANDLE*, LPVOID*);

VOID freeMappedMemoryDLL(HANDLE, LPVOID, HANDLE, LPVOID);

BOOL initSyncObjectsDLL(HANDLE*, HANDLE*, HANDLE*, HANDLE*, HANDLE*);

VOID freeSyncObjectsDLL(HANDLE, HANDLE, HANDLE, HANDLE, HANDLE);
/*----------  FIM Fun�oes de Bootstrapping para memoria partilhada ----------*/

/*---------- Fun�oes de Bootstrapping para NamedPipes ----------*/
BOOL initNamedPipeLocalDLL(PipeLocal*);

VOID freeNamedPipeLocalDLL(PipeLocal*);
/*---------- FIM Fun�oes de Bootstrapping para NamedPipes ----------*/