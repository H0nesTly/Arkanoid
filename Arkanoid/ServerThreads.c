#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"
#include <minwinbase.h>

inline static VOID readNewMessageSharedMemory(MessageQueue* queue, Server* serverObj)
{
	//CRITICAL SECTION
	WaitForSingleObject(hgMutexReadNewMessage, INFINITE);

	for (; queue->wLastReadMessageIndex != queue->wLastUnReadMessageIndex;//Erro aqui
		queue->wLastReadMessageIndex = (queue->wLastReadMessageIndex + 1) % MESSAGE_QUEUE_SIZE)
	{
		_tprintf_s(TEXT("\n Valor da read %d e da Unread %d"), queue->wLastReadMessageIndex, queue->wLastUnReadMessageIndex);
		switch (queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].request)
		{
		case LoginMessage:
			//Insere na lista de jogadore
			if (addUserNameToLobby(queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].messagePD.tcSender, &serverObj->gameInstance))
			{

				//FIX: Inserir 1 jogador a mais?
				for (size_t i = 0; i < serverObj->gameInstance.lobbyGame.wPlayersInLobby; i++)
				{
					_tprintf(TEXT("\n Jogador : %s| Concectou-se %d"),
						serverObj->gameInstance.lobbyGame.playersInLobby[i].tcUserName,
						serverObj->gameInstance.lobbyGame.wPlayersInLobby);
				}

				//Escrevemos nova mensagem para o cliente 
				writeMessageToClientSharedMemory(queue,
					ResponseLoginSuccess,
					NAME_SERVER,
					queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].messagePD.tcSender);
				//VAMOS NOTIFICAR A thread Produtora
				if (!SetEvent(hgSyncRWObject))
				{
					_tprintf_s(TEXT("\n Erro set evento"));
				}
			}
			else
			{
				writeMessageToClientSharedMemory(queue,
					ResponseLoginFail,
					NAME_SERVER,
					queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].messagePD.tcDestination);
			}

			//Apagar a mensagem indica que foi lida
			ZeroMemory(&queue->queueOfMessageClientServer[queue->wLastReadMessageIndex], sizeof(MessageProtocolDatagramRequest));
			//notificamos todos os consumidores a dizer que a uma nova mensagem
			SetEvent(hgNotifyClient);
			//TODO: Esperamos X ms e damos reset no evento
					//FIM DA CRITICAL SECTION
			if (!ReleaseMutex(hgMutexReadNewMessage))
			{
				//erro lançar mutex
			}
			break;
		case TopPlayersMessage:
			break;
		case QuitGameMessage:
			break;
		default:
			break;
		}
	}
}

inline static VOID readNewMessageNamedPipes(Server* serverObj, MessageProtocolPipe * messageReceive, MessageProtocolPipe* messageToSend, WORD wIndex)
{
	NamedPipeInstance* npInstances = (NamedPipeInstance*)serverObj->serverHandlers.namedPipeInstances;
	DWORD dwNumberOfBytesRead, dwBytesReadAsync;
	BOOL bOverLapped, bOperationReturn;

	if (npInstances[wIndex].fPendigIO)
	{
		bOverLapped = GetOverlappedResult(
			npInstances[wIndex].hNPInstance,
			&npInstances[wIndex].oOverLap,
			&dwNumberOfBytesRead,
			FALSE);

		switch (npInstances[wIndex].State)
		{
		case WriteState:
		case ReadState:
		case ConnectingState:
			if (!bOverLapped)
			{
				switch (GetLastError())
				{
					//Pode acontecer quando existe muitos pedios de I/O Assincrono
				case ERROR_INVALID_USER_BUFFER:
				case ERROR_NOT_ENOUGH_MEMORY:
					_tprintf(TEXT("\nErro Critico [%d]"), GetLastError());
					break;
				case ERROR_BROKEN_PIPE:
					_tprintf(TEXT("\nPipeDesconectou-se Erro[%d]"), GetLastError());
					break;
					//Funçao Cancelada
				case ERROR_OPERATION_ABORTED:
					_tprintf(TEXT("\nOperaçao cancelada Erro Critico [%d]"), GetLastError());
					break;
				default:
					_tprintf(TEXT("\nERRO default [%d]"), GetLastError());
					break;
				}
				DisconnectAndReconnect(&npInstances[wIndex]);
				_tprintf(TEXT("\nErro Pending Coneçao [%d]"), GetLastError());
				return;
			}
			break;
			//case ReadState:
			//	if (!bOverLapped || dwNumberOfBytesRead == 0)
			//	{
			//		DisconnectNamedPipe(&npInstances[i].hNPInstance);
			//		_tprintf(TEXT("Erro Pending ler [%d]"), GetLastError());
			//		continue;
			//	}
			//	npInstances[i].State = ReadState;
			//	break;
				//case WriteState:
				//	if (!bOverLapped )
				//	{
				//		DisconnectNamedPipe(&npInstances[i].hNPInstance);
				//		_tprintf(TEXT("Erro Pending escrever [%d]"), GetLastError());
				//		continue;
				//	}
				//	npInstances[i].State = ReadState;
				//	break;
		default:
			_tprintf(TEXT("Erro  Undefined pipesate [%d]"), GetLastError());
			break;
		}
	}

	switch (npInstances[wIndex].State)
	{
	case ConnectingState:
	case ReadState:

		bOperationReturn = ReadFile(
			npInstances[wIndex].hNPInstance, //handler de onde vai ler
			messageReceive,			//destino
			sizeof(MessageProtocolPipe) + 200, //tamanho da mensagem
			&dwBytesReadAsync,						//valor de onde vai guardar valores lidos
			&npInstances[wIndex].oOverLap);	//atualiza estado

		_tprintf(TEXT("\n Handle [%p] Nome %s Erro[%d]\n"), npInstances[wIndex].hNPInstance, messageReceive->messagePD.tcSender, GetLastError());

		// The read operation completed successfully. 

		if (bOperationReturn  && dwBytesReadAsync != 0)
		{
			//if(addUserNameToLobby(queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].messagePD.tcSender, &serverObj->gameInstance))
			//{
			//	
			//}
			npInstances[wIndex].fPendigIO = FALSE;
			npInstances[wIndex].State = WriteState;
			return;
		}

		// The read operation is still pending. 

		if (!bOperationReturn && (GetLastError() == ERROR_IO_PENDING))
		{
			npInstances[wIndex].fPendigIO = TRUE;
			return;
		}
		break;
	case WriteState:
		//bOperationReturn = WriteFile(npInstances[wIndex].hNPInstance,
		//	,);

		if (! bOperationReturn && (GetLastError() == ERROR_IO_PENDING)) 
            { 
               Pipe[i].fPendingIO = TRUE; 
               continue; 
            }
		break;
	}
}

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* queue = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;
	MessageProtocolPipe messageReceive;
	MessageProtocolPipe messageToSend;

	DWORD dwWaitEvent;

	HANDLE hAllHandlers[SIZE_OF_HANDLERS_READ];
	int i;

	for (int i = 0; i <= INDEX_OF_HANDLERS_NAMEDPIPE; ++i)
	{
		hAllHandlers[i] = serverObj->serverHandlers.namedPipeInstances[i].hMyEvent;
	}

	hAllHandlers[INDEX_OF_HANDLERS_WAIT_MESSAGE] = hgSyncSemaphoreRead;

	ZeroMemory(&messageReceive, sizeof(MessageProtocolPipe));
	ZeroMemory(&messageToSend, sizeof(MessageProtocolPipe));

	while (1)
	{
		_tprintf_s(TEXT("\nA espera de Clientes para se conectar ..."));

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
			readNewMessageNamedPipes(serverObj, &messageReceive, &messageToSend, i);
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
	Game* game = (Game*)lpArg;

	//while (1)
	//{
	//	Sleep(1000); //Remover apenas para exemplo
	//	moveBall(&game->ball);
	//	_tprintf(TEXT("\nBola x-%d"), game->ball.ballPosition.x);
	//}

	return 0;
}

VOID freeThreads(ServerThreadsHandlers handlers)
{
	CloseHandle(handlers.hThreadConsumer);
	CloseHandle(handlers.hThreadBall);
}