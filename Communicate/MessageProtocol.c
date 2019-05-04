#include "stdafx.h"
#include "MessageProtocol.h"

extern LPVOID lpgSharedMemMessage;

extern HANDLE hgWriteObject;
extern HANDLE hgReadObject;

VOID __cdecl Login(LPVOID  lpSharedMem, PTCHAR username)
{

	lpSharedMem = lpgSharedMemMessage;

	MessageQueue* queue = (MessageQueue*)lpSharedMem;

	//MUTEX ou sincronização quando se escreve no buffer		
	_tcscpy_s(queue->queueOfMessageClientServer[0].messagePD.tcSender,
		(sizeof(TCHAR) * MAX_LENGTH_NAME),
		username);

	_tcscpy_s(queue->queueOfMessageClientServer[0].messagePD.tcDestination,
		(sizeof(TCHAR) * MAX_LENGTH_NAME),
		NAME_SERVER);

	queue->queueOfMessageClientServer[0].request= LoginMessage;

	_tcscpy_s(queue->queueOfMessageClientServer[0].messagePD.tcData,
		sizeof(TCHAR) * MAX_LENGTH_NAME,
			TEXT("Quero me conectar"));

	//queue->wLastUnReadMessageIndex++;

	if (!SetEvent(hgWriteObject))
	{
		_tprintf(TEXT("Set event erro (%d)\n"), GetLastError());
		return;
	}

}

VOID __cdecl ReceiveBroadcast(){}

VOID __cdecl SendMessageDll(){}

VOID __cdecl ReceiveMessage(){}