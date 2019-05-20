#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"
#include <minwinbase.h>
#include <minwinbase.h>
#include <stdlib.h>

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
			}
			else
			{
				writeMessageToClientSharedMemory(queue,
					ResponseLoginFail,
					NAME_SERVER,
					queue->queueOfMessageClientServer[queue->wLastReadMessageIndex].messagePD.tcSender);
			}

			//Apagar a mensagem indica que foi lida
			ZeroMemory(&queue->queueOfMessageClientServer[queue->wLastReadMessageIndex], sizeof(MessageProtocolDatagramRequest));

			//notificamos todos os consumidores a dizer que a uma nova mensagem
			SetEvent(hgNotifyClient);

			//TODO: Esperamos X ms e damos reset no evento
					//FIM DA CRITICAL SECTION
			if (!ReleaseMutex(hgMutexReadNewMessage))
			{
				//erro desbloquear mutex
			}
			//TODO: Set Waitable timer
			Sleep(20000);
			ResetEvent(hgNotifyClient);

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

inline static VOID readNewMessageNamedPipes(NamedPipeInstance* npInstances, Server* serverObj)
{
	DWORD dwBytesReadAsync;
	BOOL bOverLapped, bOperationReturn;

	if (npInstances->fPendigIO)
	{
		bOverLapped = GetOverlappedResult(
			npInstances->hNPInstance,
			&npInstances->oOverLap,
			&dwBytesReadAsync,
			FALSE);

		switch (npInstances->State)
		{
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
			}
				npInstances->State = ReadState;

				return;

			break;
		case WriteState:
			if (!bOverLapped || dwBytesReadAsync != sizeof(MessageProtocolPipe))
			{
				DisconnectAndReconnect(npInstances);
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
			npInstances->hNPInstance, //handler de onde vai ler
			&npInstances->message,			//destino
			sizeof(MessageProtocolPipe), //tamanho da mensagem
			&npInstances->dwNumberOfBytesRead,						//valor de onde vai guardar valores lidos
			&npInstances->oOverLap);	//atualiza estado



		// The read operation completed successfully. 

		if (bOperationReturn  && npInstances->dwNumberOfBytesRead != 0)
		{
			//processamos mensagem

			if (npInstances->message.wTypeOfMessage == TYPE_OF_MESSAGE_REQUEST)
			{
				_tprintf(TEXT("\n Handle [%p] Nome %s Erro[%d], Return %d\n"), npInstances->hNPInstance, npInstances->message.messagePD.tcSender, GetLastError(), bOperationReturn);

				switch (npInstances->message.request)
				{
				case LoginMessage:
					npInstances->message.response = ResponseLoginSuccess;
					npInstances->message.wTypeOfMessage = TYPE_OF_MESSAGE_RESPONSE;

					_tcscpy_s(npInstances->message.messagePD.tcData,
						_countof(npInstances->message.messagePD.tcData),
						TEXT("Teste Named Pipe"));
					//if (addUserNameToLobby(npInstances->message.messagePD.tcSender, &serverObj->gameInstance))
					//{
					//	//responde Sucesso
					//	ZeroMemory(&npInstances->message , sizeof(MessageProtocolPipe));
					//}
					//else
					//{
					//	//Ja existe um cliente com este nome
					//	
					//}
					break;
				case TopPlayersMessage:
					break;
				case KeyPressedMessage:
					break;
				case QuitGameMessage:
					break;
				}
			}

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
		bOperationReturn = WriteFile(
			npInstances->hNPInstance,		//File handler
			&npInstances->message,			//Destino
			sizeof(MessageProtocolPipe),	//tamanho a ler
			&dwBytesReadAsync,	//bytes lidos
			&npInstances->oOverLap);

		// The write operation completed successfully. 

		if (bOperationReturn && npInstances->dwNumberOfBytesWritten == sizeof(MessageProtocolPipe))
		{
			npInstances->fPendigIO = FALSE;
			npInstances->State = ReadState;
			_tprintf(TEXT("\n Mensagem escrita com sucesso!!"));
		}

		// The write operation is still pending. 

		if (!bOperationReturn && (GetLastError() == ERROR_IO_PENDING))
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
		_tprintf_s(TEXT("\nA espera de Clientes para se conectar ... [%d]"), GetLastError());

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