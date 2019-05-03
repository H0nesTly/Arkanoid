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
	//_tcscpy_s(queue->queueOfMessageClientServer[0].tcSender,
	//	(sizeof(TCHAR) * MAX_LENGTH_NAME),
	//	username);

	//queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].typeOfMessageData = LoginMessage;

	//_tcscpy_s(queue->queueOfMessageClientServer[0].tcData,
	//	sizeof(TCHAR) * MAX_LENGTH_NAME,
	//		TEXT("Quero me conectar"));

	queue->wLastUnReadMessageIndex = 1;

	if (!SetEvent(hgWriteObject))
	{
		_tprintf(TEXT("Set event erro (%d)\n"), GetLastError());
		return;
	}

}

VOID __cdecl ReceiveBroadcast(){}

VOID __cdecl SendMessageDll(){}

VOID __cdecl ReceiveMessage(){}