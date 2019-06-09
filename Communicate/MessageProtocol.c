#include "stdafx.h"
#include "MessageProtocol.h"
#include "GameStructures.h"
#include "CircularBuffer.h"
#include "../Client/ClientStructures.h"
#include <minwinbase.h>

extern ClientConnection gClientConnection;

static HANDLE ghNewBroadCastMessage = NULL;
/*Variavel que continua o o programa a correr*/

static VOID loginSharedMemory(const PTCHAR username)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWaitMutex;

	//MUTEX DEVE ESTAR BLOQUEADO
	//CRITICAL SECTION
	dwWaitMutex = WaitForSingleObject(gClientConnection.SharedMem.hMutexWriteNewMessage, INFINITE);

	/*Lista de mensagens está cheia*/
		//Escrevemos mensagem

	writeMessageToServerSharedMemory(queue, LoginMessage, username, NAME_SERVER);

	//END CRITICAL SECTION	

	if (!ReleaseSemaphore(gClientConnection.SharedMem.hSemaphoreWriteMessageToServer, 1, NULL) ||
		!ReleaseMutex(gClientConnection.SharedMem.hMutexWriteNewMessage))
	{
		_tprintf(TEXT("Release do mutex ou semafor FALHOU:(%d)\n"), GetLastError());
		return;
	}
}

static VOID loginLocalPIPE(const PTCHAR username)
{
	HANDLE hPipe = gClientConnection.PipeLocal.hNamedPipeWriteToServer;
	MessageProtocolPipe messageToSend;
	DWORD dwBytesToWrite;

	ZeroMemory(&messageToSend, sizeof(MessageProtocolPipe));

	writeMessageToServerPipeRequest(&messageToSend, LoginMessage, username, NAME_SERVER);

	dwBytesToWrite = sizeof(MessageProtocolPipe);
	WriteFile(hPipe,
		&messageToSend,
		dwBytesToWrite,
		&dwBytesToWrite,
		NULL);
}

static Game* receiveBroadcastSharedMemory()
{
	WaitForSingleObject(gClientConnection.SharedMem.hEventWaitForGameData, INFINITE);

	Game* game = (Game*)gClientConnection.SharedMem.lpGame;

	ResetEvent(gClientConnection.SharedMem.hEventWaitForGameData);

	return game;
}

static Game*  receiveBroadcastPipe()
{
	WaitForSingleObject(ghNewBroadCastMessage, INFINITE);

	Game *gameObj = gClientConnection.PipeLocal.gameObj;

	return gameObj;
	//CopyMemory(gameObj, &gClientConnection.PipeLocal.gameObj, sizeof(Game));
}

static VOID receiveMessageSharedMemory(const PTCHAR UserName, BOOL* bKeepRunning)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWait, wCount = 0;

	dwWait = WaitForSingleObject(gClientConnection.SharedMem.hSemaphoreReadMessageFromServer, INFINITE);
	//Critical section

	if (_tcscmp(UserName, queue->circularBufferServerClient.queueOfMessage[queue->circularBufferServerClient.wTailIndex].tcDestination) == 0 /*||
		queue->queueOfMessageServerClient[queue->wTailIndex].messagePD.tcDestination[0] == '*'*/)
	{
		switch (queue->circularBufferServerClient.queueOfMessage[queue->circularBufferServerClient.wTailIndex].request)
		{
		case ResponseLoginSuccess:

			gClientConnection.SharedMem.bAlreadyAuthenticate = TRUE;
			advanceTail(&queue->circularBufferServerClient);
			break;
		case ResponseLoginFail:
			if (!gClientConnection.SharedMem.bAlreadyAuthenticate)
			{
				advanceTail(&queue->circularBufferServerClient);
				*bKeepRunning = FALSE;
				MessageBox(gClientConnection.wndHandlers.hWndMain,
					TEXT("Login Falhou!"),
					TEXT("Login"),
					MB_OK | MB_ICONERROR);
				DestroyWindow(gClientConnection.wndHandlers.hWndMain);
			}
			break;
		case TopPlayersMessage:
			//TODO: COMO MOSTRAR NA INTERFACE GRÁFICA
			break;
		default:
			break;
		}
	}
}

static VOID	receiveMessageLocalPipe(const PTCHAR UserName, BOOL* bKeepRunning)
{
	HANDLE hPipe = gClientConnection.PipeLocal.hNamedPipeReadFromServer;
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
				*bKeepRunning = FALSE;
				MessageBox(gClientConnection.wndHandlers.hWndMain,
					TEXT("Login Falhou!"),
					TEXT("Login"),
					MB_OK | MB_ICONERROR);
				DestroyWindow(gClientConnection.wndHandlers.hWndMain);
				break;
			case ResponseLoginSuccess:
				break;
			case ResponseTop10:
				//TODO: COMO MOSTRAR NA INTERFACE GRÁFICA
				break;
			case ResponseGameData:
				CopyMemory(gClientConnection.PipeLocal.gameObj,
					&messageToReceive.messagePD.gameData,
					sizeof(Game));

				SetEvent(ghNewBroadCastMessage);
				break;
			}
		}

		if (GetLastError() != ERROR_MORE_DATA && !bSucess)
		{
			break;
		}
	} while (!bSucess);

}

static VOID sendMessageSharedMemory(const PTCHAR username, TypeOfRequestMessage tRequest)
{
	MessageQueue* queue = (MessageQueue*)gClientConnection.SharedMem.lpMessage;
	DWORD dwWaitMutex;
	//CRITICAL SECTION
	dwWaitMutex = WaitForSingleObject(gClientConnection.SharedMem.hMutexWriteNewMessage, INFINITE);

	writeMessageToServerSharedMemory(queue, tRequest, username, NAME_SERVER);

	//END CRITICAL SECTION	

	if (!ReleaseSemaphore(gClientConnection.SharedMem.hSemaphoreWriteMessageToServer, 1, NULL) ||
		!ReleaseMutex(gClientConnection.SharedMem.hMutexWriteNewMessage))
	{
		_tprintf(TEXT("Release do mutex ou semafor FALHOU:(%d)\n"), GetLastError());
		return;
	}
}

static VOID sendMessagePipe(const PTCHAR username, TypeOfRequestMessage tRequest)
{
	HANDLE hPipe = gClientConnection.PipeLocal.hNamedPipeWriteToServer;
	MessageProtocolPipe messageToSend;
	DWORD dwBytesToWrite;

	if (tRequest == QuitGameMessage)
	{
		SetEvent(ghNewBroadCastMessage);
	}

	ZeroMemory(&messageToSend, sizeof(MessageProtocolPipe));

	writeMessageToServerPipeRequest(&messageToSend, tRequest, username, NAME_SERVER);

	dwBytesToWrite = sizeof(MessageProtocolPipe);
	WriteFile(hPipe,
		&messageToSend,
		dwBytesToWrite,
		&dwBytesToWrite,
		NULL);
}

VOID __cdecl setGameObj(Game** gameObj)
{
	gClientConnection.PipeLocal.gameObj = *gameObj;
}

VOID __cdecl Login(const PTCHAR username, HWND hWndArg, HDC memDC, TypeOfClientConnection arg)
{
	gClientConnection.typeOfConnection = arg;

	/*Inicializamos shared memory / evento / NP
	 * Dependendo como o utilizador se pretende connectar
	 */
	initComponetsDLL(&gClientConnection);

	_tcscpy_s(gClientConnection.tcUserName,
		_countof(gClientConnection.tcUserName),
		username);

	gClientConnection.wndHandlers.hWndMain = hWndArg;
	gClientConnection.wndHandlers.memDC = memDC;

	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		loginSharedMemory(gClientConnection.tcUserName);
		break;
	case clientNamedPipeLocalConnection:
		loginLocalPIPE(gClientConnection.tcUserName);

		ghNewBroadCastMessage = CreateEvent(
			NULL,
			FALSE,		//modo automático
			FALSE,		//nao sinalizado
			NULL);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl ReceiveBroadcast(BOOL* bKeepRunning, Game** gameObj)
{
	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		*gameObj = receiveBroadcastSharedMemory();
		break;
	case clientNamedPipeLocalConnection:
		*gameObj = receiveBroadcastPipe();
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl SendMessageDll(BOOL* bKeepRunning, TypeOfRequestMessage tRequest)
{
	if (tRequest == QuitGameMessage)
	{
		*bKeepRunning = FALSE;
	}
	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		sendMessageSharedMemory(gClientConnection.tcUserName, tRequest);
		break;
	case clientNamedPipeLocalConnection:
		sendMessagePipe(gClientConnection.tcUserName, tRequest);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID __cdecl ReceiveMessage(const PTCHAR UserName, BOOL* bKeepRunning)
{
	switch (gClientConnection.typeOfConnection)
	{
	case clientSharedMemoryConnection:
		receiveMessageSharedMemory(gClientConnection.tcUserName, bKeepRunning);
		break;
	case clientNamedPipeLocalConnection:
		receiveMessageLocalPipe(gClientConnection.tcUserName, bKeepRunning);
		break;
	case clientNamedPipeRemoteConnection:
		break;
	default:
		break;
	}
}

VOID writeMessageToServerSharedMemory(MessageQueue* mqArg, TypeOfRequestMessage request, const PTCHAR pSender, const PTCHAR pDestination)
{
	MessageProtocolDatagram aux;
	ZeroMemory(&aux, sizeof(MessageProtocolDatagram));

	fillMessageToProtocolDatagram(
		&aux,
		pSender,
		pDestination);

	aux.request = request;

	addItemToBuffer(&mqArg->circularBufferClientServer, &aux);
}

VOID writeMessageToServerPipeRequest(MessageProtocolPipe* mppArg, TypeOfRequestMessage request, const PTCHAR pSender, const PTCHAR pDestination)
{
	mppArg->wTypeOfMessage = TYPE_OF_MESSAGE_REQUEST;

	mppArg->request = request;

	fillMessageToProtocolDatagram(
		&mppArg->messagePD,
		pSender,
		pDestination);
}

VOID fillMessageToProtocolDatagram(MessageProtocolDatagram* message_protocol_datagram, const PTCHAR pSender, const PTCHAR pDestination)
{
	_tcscpy_s(message_protocol_datagram->tcDestination,
		_countof(message_protocol_datagram->tcDestination),
		pDestination);

	_tcscpy_s(message_protocol_datagram->tcSender,
		_countof(message_protocol_datagram->tcSender),
		pSender);
}
