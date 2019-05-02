#include "ServerThreads.h"



DWORD WINAPI ProducerMessageThread(PVOID arg)
{
	UNREFERENCED_PARAMETER(arg);
	return 0;
}

DWORD WINAPI ConsumerMessageThread(PVOID arg)
{
	MessageQueue* queue = (MessageQueue*)arg;
	DWORD dwWaitEvent;

	dwWaitEvent = WaitForSingleObject(hgReadObject, INFINITE);

	while (1)
	{
		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0: 
			_tprintf_s(TEXT("%s"), queue->queueOfMessageClientServer->tcData);
			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}
	}
	return 0;	
}
