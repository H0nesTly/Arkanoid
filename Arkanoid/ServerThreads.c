#include "ServerThreads.h"
#include "Server.h"


DWORD WINAPI ProducerMessageThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);
	return 0;
}

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj= (Server*) lpArg;
	MessageQueue* queue = (MessageQueue*) serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;

	DWORD dwWaitEvent;


	/*while (1)
	{
		_tprintf_s(TEXT("\nA espera ..."));
		dwWaitEvent = WaitForSingleObject(hgWriteObject, INFINITE);

		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0: 
			//_tprintf_s(TEXT("\n Recebi De: %s Tipo : %d \n Para: %s\n A inFormaçao: %s"),
			//	queue->queueOfMessageClientServer[0].messagePD.tcSender,
			//	queue->queueOfMessageClientServer[0].request,
			//	queue->queueOfMessageClientServer[0].messagePD.tcDestination,
			//	queue->queueOfMessageClientServer[0].messagePD.tcData);
			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}

		ResetEvent(hgWriteObject);
	}*/
	return 0;	
}
