#pragma once
#include "stdafx.h"

typedef struct clientConnection ClientConnection;
typedef struct sharedMemory SharedMemory;
typedef struct pipeLocal PipeLocal;
typedef struct pipeRemote PipeRemote;

typedef enum typeOfClientConnection TypeOfClientConnection;

struct clientConnection
{
	TypeOfClientConnection typeOfConnection;

	union 
	{
		struct sharedMemory
		{
			//Ponteiros e handles para Memoria virtula
			LPVOID lpMessage;
			HANDLE hMessage;
			LPVOID lpGame;
			HANDLE hGame;
			/*---Eventos e mecanismos de sincronização---*/
			/*Semaforo para notificar que existe uma nova mensagem para o servidor ler*/
			HANDLE hSemaphoreWriteMessageToServer;
			/*Evento para notificar aos clientes que existe uma nova mensagem*/
			HANDLE hEventReadNewMessage;
			/*Mutex's para a escrita de novas mensagens*/
			HANDLE hMutexWriteNewMessage;
		} SharedMem;

		struct pipeLocal
		{
			LPVOID x;
		} PipeLocal;

		struct pipeRemote
		{
			LPVOID y;
		} PipeRemote;
	};
};

enum typeOfClientConnection
{
	clientSharedMemoryConnection,
	clientNamedPipeLocalConnection,
	clientNamedPipeRemoteConnection
};


BOOL initComponets(ClientConnection* );

VOID freeComponents(ClientConnection*);
/*---------- Funçoes de Bootstrapping para memoria partilhada ----------*/

BOOL initSharedMemory(SharedMemory*);

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMem(HANDLE*, LPVOID*);

/*Cria handle p da memoria mapeada*/
BOOL initClientMessageMem(HANDLE*, LPVOID*);

VOID freeMappedMemory(HANDLE, LPVOID, HANDLE, LPVOID);

BOOL initSyncObjects(HANDLE*, HANDLE*, HANDLE*);

VOID freeSyncObjects(HANDLE, HANDLE, HANDLE);
/*----------  FIM Funçoes de Bootstrapping para memoria partilhada ----------*/