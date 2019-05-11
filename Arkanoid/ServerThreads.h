#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"

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