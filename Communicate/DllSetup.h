#pragma once
#include "stdafx.h"
#include "GameStructures.h"

typedef struct clientConnection ClientConnection;
typedef struct windowhandlers Windowhandlers;
typedef struct sharedMemory SharedMemory;
typedef struct pipe Pipe;
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
			/*---Eventos e mecanismos de sincronização---*/
			/*Semaforo para notificar que existe uma nova mensagem para o servidor ler*/
			HANDLE hSemaphoreWriteMessageToServer;

			HANDLE hSemaphoreReadMessageFromServer;
			/*Evento para notificar aos clientes que existe uma nova mensagem*/
			HANDLE hEventReadNewMessage;
			/*Mutex's para a escrita de novas mensagens*/
			HANDLE hMutexWriteNewMessage;

			HANDLE hEventWaitForGameData;

		} SharedMem;

		struct pipe
		{
			HANDLE hNamedPipeWriteToServer;
			HANDLE hNamedPipeReadFromServer;
			Game* gameObj;
			TCHAR tcIP[16];
			//Estado?
		} Pipe;

	};
};

enum typeOfClientConnection
{
	clientSharedMemoryConnection,
	clientNamedPipeConnection
};


BOOL initComponetsDLL(ClientConnection*);

VOID freeComponentsDLL(ClientConnection*);
/*---------- Funçoes de Bootstrapping para memoria partilhada ----------*/

BOOL initSharedMemoryDLL(SharedMemory*);

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMemDLL(HANDLE*, LPVOID*);

/*Cria handle p da memoria mapeada*/
BOOL initClientMessageMemDLL(HANDLE*, LPVOID*);

VOID freeMappedMemoryDLL(HANDLE, LPVOID, HANDLE, LPVOID);

BOOL initSyncObjectsDLL(HANDLE*, HANDLE*, HANDLE*, HANDLE*, HANDLE*);

VOID freeSyncObjectsDLL(HANDLE, HANDLE, HANDLE, HANDLE, HANDLE);
/*----------  FIM Funçoes de Bootstrapping para memoria partilhada ----------*/

/*---------- Funçoes de Bootstrapping para NamedPipes ----------*/
BOOL initNamedPipeDLL(Pipe*);

VOID freeNamedPipeDLL(Pipe*);
/*---------- FIM Funçoes de Bootstrapping para NamedPipes ----------*/

VOID initSecurityAtributes(SECURITY_ATTRIBUTES*);