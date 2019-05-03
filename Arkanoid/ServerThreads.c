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


	while (1)
	{
		_tprintf_s(TEXT("\nA espera ..."));
		dwWaitEvent = WaitForSingleObject(hgWriteObject, INFINITE);

		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0: 
			//_tprintf_s(TEXT("\n %s | %s"), queue->queueOfMessageClientServer[0].tcData, queue->queueOfMessageClientServer[0].tcSender);
			_tprintf_s(TEXT("\n %d"), queue->wLastUnReadMessageIndex);
			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}
	}
	return 0;	
}
