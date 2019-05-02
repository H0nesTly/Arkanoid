#include "stdafx.h"
#include "MessageProtocol.h"

extern HANDLE hgWriteObject;
extern HANDLE hgReadObject;

VOID __cdecl Login(LPVOID  lpSharedMem, PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)lpSharedMem;

	//MUTEX ou sincronização quando se escreve no buffer		
	_tcscpy_s(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].tcSender,
		(sizeof(TCHAR) * MAX_LENGTH_NAME),
		username);

	queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].typeOfMessageData = LoginMessage;

	_tcscpy_s(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].tcData,
		sizeof(TCHAR) * MAX_LENGTH_NAME,
			TEXT("Quero me conectar"));

	queue->wLastUnReadMessageIndex++;

	if (!SetEvent(hgWriteObject))
	{
		_tprintf(TEXT("Set event erro (%d)\n"), GetLastError());
	}
}

VOID __cdecl ReceiveBroadcast(){}

VOID __cdecl SendMessageDll(){}

VOID __cdecl ReceiveMessage(){}