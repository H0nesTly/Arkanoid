#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"
#include  "..\Communicate\GameStructures.h"		

#define INDEX_OF_HANDLERS_NAMEDPIPE (MAX_PLAYER_INSTANCES )
#define INDEX_OF_HANDLERS_WAIT_MESSAGE (INDEX_OF_HANDLERS_NAMEDPIPE + 1)
#define SIZE_OF_HANDLERS_READ (INDEX_OF_HANDLERS_WAIT_MESSAGE + 1)

extern HANDLE hgNotifyClient;
extern HANDLE hgWriteObject;
extern HANDLE hgSyncRWObject;
extern HANDLE hgSyncSemaphoreRead;
extern HANDLE hgMutexReadNewMessage;

typedef struct serverThreadsHandlers ServerThreadsHandlers;

struct serverThreadsHandlers
{
	HANDLE hThreadConsumer;
	DWORD dwIdConsumer;
	HANDLE hThreadBall;
	DWORD dwThreadBall;
};

DWORD WINAPI ConsumerMessageThread(LPVOID);

DWORD WINAPI BallThread(LPVOID);

VOID freeThreads(ServerThreadsHandlers);