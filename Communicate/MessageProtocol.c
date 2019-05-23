#include "stdafx.h"
#include "MessageProtocol.h"
#include "GameStructures.h"
#include "CircularBuffer.h"

extern ClientConnection gClientConnection;

static VOID loginSharedMemory(const PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWaitMutex;
	WORD wNextIndexMessage;

	//MUTEX DEVE ESTAR BLOQUEADO
	//CRITICAL SECTION
	dwWaitMutex = WaitForSingleObject(gClientConnection.SharedMem.hMutexWriteNewMessage, INFINITE);

	wNextIndexMessage = (queue->circularBufferClientServer.wHeadIndex + 1) % MESSAGE_QUEUE_SIZE;

	//TODO :Se BUFFER ESTIVER CHEIO O QUE FAZER? TENTAR EM X ms apos tentativa??
	/*Lista de mensagens está cheia*/
		//Escrevemos mensagem

	_tcscpy_s(queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wHeadIndex].tcSender,
		_countof(queue->circularBufferClientServer.queueOfMessage[0].tcSender),
		username);

	_tcscpy_s(queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wHeadIndex].tcDestination,
		_countof(queue->circularBufferClientServer.queueOfMessage[0].tcDestination),
		NAME_SERVER);

	queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wHeadIndex].request = LoginMessage;

	_tcscpy_s(queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wHeadIndex].tcData,
		_countof(queue->circularBufferClientServer.queueOfMessage[0].tcData),
		TEXT("Quero me conectar"));

	queue->circularBufferClientServer.wHeadIndex = wNextIndexMessage;

	//END CRITICAL SECTION	
	//queue->wHeadIndex++;

	if (!ReleaseSemaphore(gClientConnection.SharedMem.hSemaphoreWriteMessageToServer, 1, NULL) ||
		!ReleaseMutex(gClientConnection.SharedMem.hMutexWriteNewMessage))
	{
		_tprintf(TEXT("Release do mutex ou semafor FALHOU:(%d)\n"), GetLastError());
		return;
	}
}

static VOID loginLocalPIPE(const PTCHAR username)
{
	HANDLE hPipe = gClientConnection.PipeLocal.hNamedPipe;
	MessageProtocolPipe messageToSend;
	DWORD dwBytesToWrite;

	ZeroMemory(&messageToSend, sizeof(MessageProtocolPipe));

	_tcscpy_s(messageToSend.messagePD.tcSender,
		_countof(messageToSend.messagePD.tcSender),
		username);

	_tcscpy_s(messageToSend.messagePD.tcDestination,
		_countof(messageToSend.messagePD.tcDestination),
		NAME_SERVER);

	messageToSend.wTypeOfMessage = TYPE_OF_MESSAGE_REQUEST;
	messageToSend.request = LoginMessage;

	_tcscpy_s(messageToSend.messagePD.tcData,
		_countof(messageToSend.messagePD.tcData),
		TEXT("Quero me conectar"));

	dwBytesToWrite = sizeof(MessageProtocolPipe);
	if (WriteFile(hPipe,
		&messageToSend,
		dwBytesToWrite,
		&dwBytesToWrite,
		NULL))
	{
		_tprintf(TEXT("\nMensaagem enviada com sucesso tamanho %d |Erro %d\n"), dwBytesToWrite, GetLastError());
	}
}

static VOID  receiveBroadcastSharedMemory()
{
	Game* game = (Game*)gClientConnection.SharedMem.lpGame;

	_tprintf(TEXT("Posicaçao da bola %d\n"), game->ball.ballPosition.x);
}

static VOID receiveMessageSharedMemory(const PTCHAR UserName)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWait, wCount = 0;

	dwWait = WaitForSingleObject(gClientConnection.SharedMem.hSemaphoreReadMessageFromServer, INFINITE);
	//Critical section
	_tprintf(TEXT("\n%d|%d"), WAIT_OBJECT_0, dwWait);
	if (_tcscmp(UserName, queue->circularBufferServerClient.queueOfMessage[queue->circularBufferServerClient.wTailIndex].tcDestination) == 0 /*||
		queue->queueOfMessageServerClient[queue->wTailIndex].messagePD.tcDestination[0] == '*'*/)
	{
		switch (queue->circularBufferServerClient.queueOfMessage[queue->circularBufferServerClient.wTailIndex].request)
		{
		case ResponseLoginSuccess:
			_tprintf(TEXT("Login Bem Sucedido\n"));
			gClientConnection.SharedMem.bAlreadyAuthenticate = TRUE;
			advanceTail(&queue->circularBufferServerClient);
			break;
		case ResponseLoginFail:
			if (!gClientConnection.SharedMem.bAlreadyAuthenticate)
			{
				_tprintf(TEXT("Login MAL Sucedido\n"));
				advanceTail(&queue->circularBufferServerClient);
			}
			break;
		default:
			break;
		}
		//ReleaseSemaphore()
	}
}

static VOID	receiveMessageLocalPipe(const PTCHAR UserName)
{
	HANDLE hPipe = gClientConnection.PipeLocal.hNamedPipe;
	MessageProtocolPipe messageToReceive;

	DWORD dwBytesToRead;
	BOOL bSucess;

	do
	{
		ZeroMemory(&messageToReceive, sizeof(MessageProtocolPipe));
		dwBytesToRead = sizeof(MessageProtocolPipe);

		bSucess = ReadFile(hPipe,
			&messageToReceive,
			dwBytesToRead,
			&dwBytesToRead,
			NULL);

		if (messageToReceive.wTypeOfMessage == TYPE_OF_MESSAGE_RESPONSE)
		{
			switch (messageToReceive.response)
			{
			case ResponseLoginFail:
				_tprintf(TEXT("\n%s"), messageToReceive.messagePD.tcData);
				break;
			case ResponseLoginSuccess:
				_tprintf(TEXT("\n%s"), messageToReceive.messagePD.tcData);

				break;
			}
		}

		_tprintf(TEXT("\nMensaagem recebida com sucesso tamanho %d |Erro %d\n"), dwBytesToRead, GetLastError());

		if (GetLastError() != ERROR_MORE_DATA && !bSucess)
		{
			break;
		}
	} while (!bSucess);

}

VOID __cdecl Login(PTCHAR username, TypeOfClientConnection arg)
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
		loginLocalPIPE(username);
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
		receiveMessageLocalPipe(UserName);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}




}