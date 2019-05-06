#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"

extern HANDLE hgReadObject;
extern HANDLE hgWriteObject;
extern HANDLE hgSyncRWObject;

typedef struct serverThreadsHandlers ServerThreadsHandlers;

struct serverThreadsHandlers
{
	HANDLE hThreadProducer;
	DWORD dwIdProducer;
	HANDLE hThreadConsumer;
	DWORD dwIdConsumer;
	HANDLE hThreadBall;
	DWORD dwThreadBall;
};

DWORD WINAPI ProducerMessageThread(LPVOID);

DWORD WINAPI ConsumerMessageThread(LPVOID);

DWORD WINAPI BallThread(LPVOID);

VOID freeThreads(ServerThreadsHandlers);