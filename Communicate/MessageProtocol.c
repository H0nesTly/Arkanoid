#include "stdafx.h"
#include "MessageProtocol.h"

extern LPVOID lpgSharedMemMessage;

extern HANDLE hgWriteObject;
extern HANDLE hgReadObject;

VOID __cdecl Login(PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)lpgSharedMemMessage;

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

VOID __cdecl ReceiveMessage(const PTCHAR UserName)
{
	MessageQueue* queue = (MessageQueue*)lpgSharedMemMessage;

	WaitForSingleObject(hgReadObject, INFINITE);

	if (_tcscmp(UserName, queue->queueOfMessageServerClient[queue->wFirstUnReadMessageIndexSC].messagePD.tcDestination) == 0 ||
		queue->queueOfMessageServerClient[queue->wFirstUnReadMessageIndexSC].messagePD.tcDestination[0] == '*')
	{
		switch (queue->queueOfMessageServerClient[queue->wFirstUnReadMessageIndexSC].response)
		{
		case ResponseLoginSuccess:
			_tprintf(TEXT("Login Bem Sucedido\n"));
			break;
			case ResponseLoginFail:
				_tprintf(TEXT("Login MAL Sucedido\n"));
				break;
		default:
			break;
		}
	}

	ResetEvent(hgReadObject);
}