#include "stdafx.h"
#include "MessageProtocol.h"
#include "GameStructures.h"

VOID __cdecl Login(PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)lpgSharedMemMessage;
	DWORD dwWaitMutex;
	WORD wNextIndexMessage;

	dwWaitMutex = WaitForSingleObject(hgMutexWriteNewMessage, INFINITE);
	//CRITICAL SECTION
	
	wNextIndexMessage = (queue->wLastUnReadMessageIndex + 1) % MESSAGE_QUEUE_SIZE;

	//TODO :Se BUFFER ESTIVER CHEIO O QUE FAZER? TENTAR EM X ms apos tentativa??
	/*Lista de mensagens está cheia*/
	if (wNextIndexMessage == queue->wLastReadMessageIndex) 
	{
		//QUE FAXEMOS? COM A LISTA CHEIA?
	}
	else
	{
		//Escrevemos mensagem
		queue->wLastUnReadMessageIndex = wNextIndexMessage;

		_tcscpy_s(queue->queueOfMessageClientServer[wNextIndexMessage].messagePD.tcSender,
			_countof(queue->queueOfMessageClientServer[0].messagePD.tcSender),
			username);

		_tcscpy_s(queue->queueOfMessageClientServer[wNextIndexMessage].messagePD.tcDestination,
			_countof(queue->queueOfMessageClientServer[0].messagePD.tcDestination),
			NAME_SERVER);

		queue->queueOfMessageClientServer[wNextIndexMessage].request = LoginMessage;

		_tcscpy_s(queue->queueOfMessageClientServer[wNextIndexMessage].messagePD.tcData,
			_countof(queue->queueOfMessageClientServer[wNextIndexMessage].messagePD.tcData),
			TEXT("Quero me conectar"));
	}

	//END CRITICAL SECTION	
	//queue->wLastUnReadMessageIndex++;

	if (!ReleaseSemaphore(hgSemaphoreWriteToServer, 1, NULL) || !ReleaseMutex(hgMutexWriteNewMessage))
	{
		_tprintf(TEXT("Release do mutex ou semafor FALHOU:(%d)\n"), GetLastError());
		return;
	}

}

VOID __cdecl ReceiveBroadcast()
{
	Game* game = (Game*)lpgSharedMemGame;

	_tprintf(TEXT("Posicaçao da bola %d\n"), game->ball.ballPosition.x);
}

VOID __cdecl SendMessageDll() {}

VOID __cdecl ReceiveMessage(const PTCHAR UserName)
{
	MessageQueue* queue = (MessageQueue*)lpgSharedMemMessage;

	WaitForSingleObject(hgReadObject, INFINITE);
	//Critical section

	if (_tcscmp(UserName, queue->queueOfMessageServerClient[queue->wLastUnReadMessageIndexSC].messagePD.tcDestination) == 0 ||
		queue->queueOfMessageServerClient[queue->wLastUnReadMessageIndexSC].messagePD.tcDestination[0] == '*')
	{
		switch (queue->queueOfMessageServerClient[queue->wLastUnReadMessageIndexSC].response)
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