#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"
#include "../Communicate/MessageProtocol.h"

inline static VOID readNewMessageSharedMemory(MessageQueue* queue, Server* serverObj)
{
	//CRITICAL SECTION
	WaitForSingleObject(hgMutexReadNewMessage, INFINITE);

	for (; queue->circularBufferClientServer.wTailIndex != queue->circularBufferClientServer.wHeadIndex;
		queue->circularBufferClientServer.wTailIndex = (queue->circularBufferClientServer.wTailIndex + 1) % MESSAGE_QUEUE_SIZE)
	{
		switch (queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].request)
		{
		case LoginMessage:
			//Insere na lista de jogadores
			if (addUserNameToLobby(
				queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender,
				serverObj))
			{
				//Escrevemos nova mensagem para o cliente 
				writeMessageToClientSharedMemory(queue,
					ResponseLoginSuccess,
					NAME_SERVER,
					queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender);
			}
			else
			{
				writeMessageToClientSharedMemory(queue,
					ResponseLoginFail,
					NAME_SERVER,
					queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender);
			}

			break;
		case TopPlayersMessage:

			writeMessageToClientSharedMemory(queue,
				ResponseTop10,
				NAME_SERVER,
				queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender);

			break;
		case KeyPressedLeftMessage:
			tryToMovePaddle(queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender, serverObj, -1);
			break;
		case KeyPressedRigthMessage:
			tryToMovePaddle(queue->circularBufferClientServer.queueOfMessage[queue->circularBufferClientServer.wTailIndex].tcSender, serverObj, 1);
			break;
		case QuitGameMessage:
			//Retirar cliente do lobby/lista de jogadores
			_tprintf(TEXT("fechar"));
			break;
		default:
			break;
		}
	}
	//notificamos todos os consumidores a dizer que a uma nova mensagem
	//TODO: Modificar o valor de 4 para as pessas em lobby
	ReleaseSemaphore(hgSemaphoreNotifyClientNewMessage, getPlayersInLobby(&serverObj->gameInstance.lobbyGame) + 1, NULL);

	//FIM DA CRITICAL SECTION
	if (!ReleaseMutex(hgMutexReadNewMessage))
	{
		_tprintf(TEXT("Erro a desbloquar Mutex %d"), GetLastError());
	}
}

//TODO: MELHORAR mecanismo
inline static VOID readNewMessageNamedPipes(NamedPipeInstance* npInstances, Server* serverObj)
{
	DWORD dwBytesReadAsync;
	BOOL bOverLapped, bOperationReturn;

	if (npInstances->fPendigIO)
	{
		bOverLapped = GetOverlappedResult(
			npInstances->hNamedPipeReadFromClient,
			&npInstances->oOverLapReader,
			&dwBytesReadAsync,
			FALSE);

		switch (npInstances->State)
		{
		case ConnectingState:
			if (!bOverLapped)
			{
				switch (GetLastError())
				{
					//Pode acontecer quando existe muitos pedidos de I/O Assincrono
				case ERROR_INVALID_USER_BUFFER:
				case ERROR_NOT_ENOUGH_MEMORY:
					_tprintf(TEXT("\nErro Critico [%d]"), GetLastError());
					break;
				case ERROR_BROKEN_PIPE:
					_tprintf(TEXT("\nPipe Desconectou-se Erro[%d]"), GetLastError());
					break;
					//Funçao Cancelada
				case ERROR_OPERATION_ABORTED:
					_tprintf(TEXT("\nOperaçao cancelada Erro Critico [%d]"), GetLastError());
					break;
				default:
					_tprintf(TEXT("\nERRO default [%d]"), GetLastError());
					break;
				}
				return;
			}
			npInstances->State = ReadState;

			break;
		case ReadState:
			if (!bOverLapped || dwBytesReadAsync == 0)
			{
				DisconnectAndReconnect(npInstances);
				return;
			}
			npInstances->dwNumberOfBytesRead = dwBytesReadAsync;
			npInstances->State = WriteState;
			break;

		default:
			_tprintf(TEXT("Erro  Undefined pipesate [%d]"), GetLastError());
			break;
		}
	}

	switch (npInstances->State)
	{
	case ReadState:

		bOperationReturn = ReadFile(
			npInstances->hNamedPipeReadFromClient, //handler de onde vai ler
			&npInstances->message,			//destino
			sizeof(MessageProtocolPipe), //tamanho da mensagem
			&npInstances->dwNumberOfBytesRead,						//valor de onde vai guardar valores lidos
			&npInstances->oOverLapReader);	//atualiza estado


		// The read operation completed successfully. 

		if (bOperationReturn  && npInstances->dwNumberOfBytesRead != 0)
		{
			//processamos mensagem

			npInstances->fPendigIO = FALSE;
			npInstances->State = WriteState;
			return;
		}

		// The read operation is still pending. 

		if (!bOperationReturn && (GetLastError() == ERROR_IO_PENDING))
		{
			npInstances->fPendigIO = TRUE;
			return;
		}

		break;
	case WriteState:

		if (npInstances->message.wTypeOfMessage == TYPE_OF_MESSAGE_REQUEST)
		{
			_tprintf(TEXT("\n Handle [] Nome %s Erro[%d]\n"), npInstances->message.messagePD.tcSender, GetLastError());

			switch (npInstances->message.request)
			{
			case LoginMessage:

				npInstances->message.wTypeOfMessage = TYPE_OF_MESSAGE_RESPONSE;

				if (addUserNameToLobby(npInstances->message.messagePD.tcSender, serverObj))
				{
					//responde Sucesso
					ZeroMemory(&npInstances->message, sizeof(MessageProtocolPipe));

					npInstances->message.response = ResponseLoginSuccess;

					_tcscpy_s(npInstances->message.messagePD.tcData,
						_countof(npInstances->message.messagePD.tcData),
						TEXT("Login Bem Sucessido"));
				}
				else
				{
					//Ja existe um cliente com este nome
					ZeroMemory(&npInstances->message, sizeof(MessageProtocolPipe));

					npInstances->message.response = ResponseLoginFail;

					_tcscpy_s(npInstances->message.messagePD.tcData,
						_countof(npInstances->message.messagePD.tcData),
						TEXT("Login Mal Sucessido"));
				}
				break;
			case TopPlayersMessage:
				break;
			case KeyPressedLeftMessage:
				tryToMovePaddle(npInstances->message.messagePD.tcSender, serverObj, -1);
				break;
			case KeyPressedRigthMessage:
				tryToMovePaddle(npInstances->message.messagePD.tcSender, serverObj, 1);
				break;
			case QuitGameMessage:
				break;
			}
		}

		ConnectNamedPipe(npInstances->hNamedPipeWriteToClient, NULL);

		bOperationReturn = WriteFile(
			npInstances->hNamedPipeWriteToClient,		//File handler
			&npInstances->message,			//Destino
			sizeof(MessageProtocolPipe),	//tamanho a ler
			&dwBytesReadAsync,	//bytes lidos
			NULL);

		// The write operation completed successfully. 

		if (bOperationReturn && dwBytesReadAsync == sizeof(MessageProtocolPipe))
		{
			npInstances->fPendigIO = FALSE;
			npInstances->State = ReadState;
			_tprintf(TEXT("\n Mensagem escrita com sucesso!!"));
			return;
		}

		// The write operation is still pending. 

		if (bOperationReturn && (GetLastError() == ERROR_IO_PENDING))
		{
			npInstances->fPendigIO = TRUE;
			return;
		}

		break;
	default:
		_tprintf(TEXT("\n Estado não esperado"));
		break;
	}
}

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* queue = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;
	NamedPipeInstance*  npInstance = (NamedPipeInstance*)serverObj->serverHandlers.namedPipeInstances;

	DWORD dwWaitEvent;

	HANDLE hAllHandlers[SIZE_OF_HANDLERS_READ];
	int i;

	for (int i = 0; i <= INDEX_OF_HANDLERS_NAMEDPIPE; ++i)
	{
		hAllHandlers[i] = serverObj->serverHandlers.namedPipeInstances[i].hMyEvent;
		ZeroMemory(&serverObj->serverHandlers.namedPipeInstances[i].message, sizeof(MessageProtocolPipe));
	}

	hAllHandlers[INDEX_OF_HANDLERS_WAIT_MESSAGE] = hgSyncSemaphoreRead;

	while (1)
	{
		//_tprintf_s(TEXT("\nA espera de Clientes para se conectar ... [%d]"), GetLastError());

		dwWaitEvent = WaitForMultipleObjects(
			SIZE_OF_HANDLERS_READ, //Numero de handlers
			hAllHandlers,		//eventos para estar a espera
			FALSE,				//Nao espera por todos
			INFINITE);

		i = WAIT_OBJECT_0 + dwWaitEvent;
		if (i < 0 || i > SIZE_OF_HANDLERS_READ - 1)
		{
			_tprintf(TEXT("\nErro out of range"));
			continue;
		}

		if (i < INDEX_OF_HANDLERS_NAMEDPIPE)
		{
			readNewMessageNamedPipes(npInstance + i, serverObj);
		}
		else
		{
			if (i >= INDEX_OF_HANDLERS_NAMEDPIPE)
			{
				//foi semaforo(Memoria Partilhada)
				readNewMessageSharedMemory(queue, serverObj);
			}
			_tprintf(TEXT("\nErro %d, WAIT %d\n"), GetLastError(), dwWaitEvent);
		}
	}
	return 0;
}

DWORD WINAPI BallThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	Game* game = (Game*)serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame;

	HANDLE hTimerWaitForPlayersToConnect = NULL;
	HANDLE hTimerWaitUpdateBall = NULL;
	LARGE_INTEGER liDueTime;
	LARGE_INTEGER liDueTimeBall;

	liDueTime.QuadPart = -20000000LL; // 5 SEGUNDOS
	liDueTimeBall.QuadPart = -1100000LL;//500 ms


	hTimerWaitForPlayersToConnect = CreateWaitableTimer(NULL, TRUE, NULL);
	hTimerWaitUpdateBall = CreateWaitableTimer(NULL, TRUE, NULL);

	if (hTimerWaitForPlayersToConnect == NULL || hTimerWaitUpdateBall == NULL)
	{
		_tprintf(TEXT("CreateWaitableTimer failed (%d)\n"), GetLastError());
		return 1;
	}

	_tprintf(TEXT("\nVamos inicializar o Jogo em 5 segundos \n"));

	// Set a timer to wait for 5 seconds.
	if (!SetWaitableTimer(
		hTimerWaitForPlayersToConnect, // Handle do timer
		&liDueTime,			//Tempo
		FALSE,				//não é periodico
		NULL,				//Completion Ruoutime
		NULL,				//Argumentos para routime
		FALSE))
	{
		printf("SetWaitableTimer failed (%d)\n", GetLastError());
		return 2;
	}


	createLevel(serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame);
	//enquanto espera por mais jogadores se conectarem carrega o jogo 

	WaitForSingleObject(hTimerWaitForPlayersToConnect, INFINITE);

	_tprintf(TEXT("\nTranferir playesrs"));
	transferPlayersToGame(serverObj);
	SetEvent(hgGameObject);

	while (1)
	{
		//When a manual-reset timer is set to the signaled state, it remains in this state until SetWaitableTimer is
		//called to reset the timer. As a result, a periodic manual-reset timer is set to the signaled state 
		//when the initial due time arrives and remains signaled until it is reset. When a synchronization timer is set to the signaled state, 
		//it remains in this state until a thread completes a wait operation on the timer object.
		SetWaitableTimer(hTimerWaitUpdateBall,
			&liDueTimeBall,
			TRUE,	//PERIOICO
			NULL,
			NULL,
			FALSE);

		WaitForSingleObject(hTimerWaitUpdateBall, INFINITE);
		moveBall(game);
		SetEvent(hgGameObject);
	}

	return 0;
}

VOID freeThreads(ServerThreadsHandlers handlers)
{
	CloseHandle(handlers.hThreadConsumer);
	CloseHandle(handlers.hThreadBall);
}