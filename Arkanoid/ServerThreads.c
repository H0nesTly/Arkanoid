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
			_tprintf_s(TEXT("\n Recebi De: %s Tipo : %d \n Para: %s\n A inFormaçao: %s"),
				queue->queueOfMessageClientServer[0].messagePD.tcSender,
				queue->queueOfMessageClientServer[0].request,
				queue->queueOfMessageClientServer[0].messagePD.tcDestination,
				queue->queueOfMessageClientServer[0].messagePD.tcData);
			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}

		ResetEvent(hgWriteObject);
	}
	return 0;	
}
