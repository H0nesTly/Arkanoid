#pragma once
#include "..\Communicate\stdafx.h"

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
