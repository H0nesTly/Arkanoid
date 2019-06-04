#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"
#include  "..\Communicate\GameStructures.h"		

#define INDEX_OF_HANDLERS_NAMEDPIPE (MAX_PLAYER_INSTANCES )
#define INDEX_OF_HANDLERS_WAIT_MESSAGE (INDEX_OF_HANDLERS_NAMEDPIPE)
#define SIZE_OF_HANDLERS_READ (INDEX_OF_HANDLERS_WAIT_MESSAGE + 1)

extern HANDLE hgNotifyClient;
extern HANDLE hgWriteObject;
extern HANDLE hgGameObject;
extern HANDLE hgSyncRWObject;
extern HANDLE hgSyncSemaphoreRead;

extern HANDLE hgMutexReadNewMessage;
extern HANDLE hgSemaphoreNotifyClientNewMessage;

typedef struct serverThreadsHandlers ServerThreadsHandlers;

struct serverThreadsHandlers
{
	HANDLE hThreadConsumer;
	DWORD dwIdConsumer;

	HANDLE hThreadBall;
	DWORD dwThreadBall;

	HANDLE hThreadBonus;
	DWORD dwThreadBonus;
};

DWORD WINAPI ConsumerMessageThread(LPVOID);

DWORD WINAPI BallThread(LPVOID);

DWORD WINAPI BonusThread(LPVOID);

VOID freeThreads(ServerThreadsHandlers);