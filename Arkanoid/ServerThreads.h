#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"

extern HANDLE hgReadObject;
extern HANDLE hgWriteObject;


typedef struct serverThreadsHandlers ServerThreadsHandlers;

struct serverThreadsHandlers
{
	HANDLE hThreadProducer;
	DWORD dwIdProducer;
	HANDLE hThreadConsumer;
	DWORD dwIdConsumer;
};

DWORD WINAPI ProducerMessageThread(PVOID);

DWORD WINAPI ConsumerMessageThread(PVOID);
