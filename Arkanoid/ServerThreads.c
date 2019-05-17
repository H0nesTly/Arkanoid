#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"

static void readNewMessageSharedMemory(MessageQueue* queue, Server* serverObj)
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
				writeMessageToClient(queue,
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
				writeMessageToClient(queue,
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

static void readNewMessageNamedPipes(Server* serverObj)
{

}

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* queue = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;
	NamedPipeInstance* npInstances = (NamedPipeInstance*)serverObj->serverHandlers.namedPipeInstances;
	MessageProtocolPipe mppAux;

	DWORD dwNumberOfBytesRead;
	DWORD dwWaitEvent;
	HANDLE hAllHandlers[SIZE_OF_HANDLERS_READ];
	int i;

	for (int i = 0; i <= INDEX_OF_HANDLERS_NAMEDPIPE; ++i)
	{
		hAllHandlers[i] = serverObj->serverHandlers.namedPipeInstances[i].oOverLap.hEvent;
	}

	hAllHandlers[INDEX_OF_HANDLERS_WAIT_MESSAGE] = hgSyncSemaphoreRead;

	while (1)
	{
		_tprintf_s(TEXT("\nA espera de Clientes para se conectar ..."));
		//dwWaitEvent = WaitForSingleObject(hgSyncSemaphoreRead, INFINITE);

		dwWaitEvent = WaitForMultipleObjects(
			SIZE_OF_HANDLERS_READ, //Numero de handlers
			hAllHandlers,		//eventos para estar a espera
			FALSE,				//Nao espera por todos
			INFINITE);


		_tprintf_s(TEXT("\nValor do dwait %d | index %d | Erro %d"),
			dwWaitEvent,
			WAIT_OBJECT_0 + dwWaitEvent,
			GetLastError());

		i = WAIT_OBJECT_0 + dwWaitEvent;
		if (i < 0 || i > SIZE_OF_HANDLERS_READ - 1)
		{
			_tprintf(TEXT("\nErro out of range"));
			continue;
		}

		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0:
			if (i == INDEX_OF_HANDLERS_WAIT_MESSAGE)
			{
				//foi semaforo(Memoria Partilhada)
				readNewMessageSharedMemory(queue, serverObj);
			}
			else
			{
				DWORD dwAux;
				//Foi namedpipe
				GetOverlappedResult(
					npInstances[i].hNPInstance,
					&npInstances[i].oOverLap,
					&dwNumberOfBytesRead,
					FALSE
				);

				ReadFile(
					npInstances[i].hNPInstance,
					&mppAux,
					sizeof(MessageProtocolPipe),
					&dwAux,
					&npInstances[i].oOverLap);

				//ResetEvent(npInstances[i].oOverLap.hEvent);
				_tprintf(TEXT("\nLi informação valor %d\nSender %s"), dwAux, mppAux.messagePD.tcSender);

			}
			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}
	}
	return 0;
}

DWORD WINAPI BallThread(LPVOID lpArg)
{
	Game* game = (Game*)lpArg;

	while (1)
	{
		Sleep(1000); //Remover apenas para exemplo
		moveBall(&game->ball);
		_tprintf(TEXT("\nBola x-%d"), game->ball.ballPosition.x);
	}

	return 0;
}

VOID freeThreads(ServerThreadsHandlers handlers)
{
	CloseHandle(handlers.hThreadConsumer);
	CloseHandle(handlers.hThreadBall);
}