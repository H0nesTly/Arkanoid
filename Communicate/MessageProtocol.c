#include "stdafx.h"
#include "MessageProtocol.h"
#include "GameStructures.h"

extern ClientConnection gClientConnection;

static VOID loginSharedMemory(const PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWaitMutex;
	WORD wNextIndexMessage;

	//MUTEX DEVE ESTAR BLOQUEADO
	//CRITICAL SECTION
	dwWaitMutex = WaitForSingleObject(gClientConnection.SharedMem.hMutexWriteNewMessage, INFINITE);

	wNextIndexMessage = (queue->wLastUnReadMessageIndex + 1) % MESSAGE_QUEUE_SIZE;

	//TODO :Se BUFFER ESTIVER CHEIO O QUE FAZER? TENTAR EM X ms apos tentativa??
	/*Lista de mensagens est� cheia*/
	if (wNextIndexMessage == queue->wLastReadMessageIndex)
	{
		//QUE FAXEMOS? COM A LISTA CHEIA?
	}
	else
	{
		//Escrevemos mensagem

		_tcscpy_s(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].messagePD.tcSender,
			_countof(queue->queueOfMessageClientServer[0].messagePD.tcSender),
			username);

		_tcscpy_s(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].messagePD.tcDestination,
			_countof(queue->queueOfMessageClientServer[0].messagePD.tcDestination),
			NAME_SERVER);

		queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].request = LoginMessage;

		_tcscpy_s(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].messagePD.tcData,
			_countof(queue->queueOfMessageClientServer[queue->wLastUnReadMessageIndex].messagePD.tcData),
			TEXT("Quero me conectar"));

		queue->wLastUnReadMessageIndex = wNextIndexMessage;
	}

	//END CRITICAL SECTION	
	//queue->wLastUnReadMessageIndex++;

	if (!ReleaseSemaphore(gClientConnection.SharedMem.hSemaphoreWriteMessageToServer, 1, NULL) ||
		!ReleaseMutex(gClientConnection.SharedMem.hMutexWriteNewMessage))
	{
		_tprintf(TEXT("Release do mutex ou semafor FALHOU:(%d)\n"), GetLastError());
		return;
	}
}

static VOID  receiveBroadcastSharedMemory()
{

	Game* game = (Game*)gClientConnection.SharedMem.lpGame;

	_tprintf(TEXT("Posica�ao da bola %d\n"), game->ball.ballPosition.x);
}

static VOID receiveMessageSharedMemory(const PTCHAR UserName)
{

	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;

	WaitForSingleObject(gClientConnection.SharedMem.hEventReadNewMessage, INFINITE);
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
	//FIX: RESET apenas 1 vai ler
	ResetEvent(gClientConnection.SharedMem.hEventReadNewMessage);
}

VOID __cdecl Login(PTCHAR username,TypeOfClientConnection arg)
{
	gClientConnection.typeOfConnection = arg;

	/*Inicializamos shared memory / evento / NP
	 * Dependendo como o utilizador se pretende connectar
	 */
	initComponetsDLL(&gClientConnection);

	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		loginSharedMemory(username);
		break;
	case clientNamedPipeLocalConnection:
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl ReceiveBroadcast()
{
	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		receiveBroadcastSharedMemory();
		break;
	case clientNamedPipeLocalConnection:
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl SendMessageDll()
{
	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		break;
	case clientNamedPipeLocalConnection:
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl ReceiveMessage(const PTCHAR UserName)
{

	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		receiveMessageSharedMemory(UserName);
		break;
	case clientNamedPipeLocalConnection:
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}