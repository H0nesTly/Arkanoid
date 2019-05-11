#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* queue = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;

	DWORD dwWaitEvent;

	while (1)
	{
		_tprintf_s(TEXT("\nA espera ..."));
		dwWaitEvent = WaitForSingleObject(hgSyncSemaphoreRead, INFINITE);

		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0:
			//CRITICAL SECTION
			WaitForSingleObject(hgMutexReadNewMessage, INFINITE);

			for (;queue->wLastReadMessageIndex == queue->wLastUnReadMessageIndex;//Erro aqui
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
							_tprintf_s(TEXT("\n Jogador : %s| Concectou-se %d"),
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
					break;
				case TopPlayersMessage:
					break;
				case QuitGameMessage:
					break;
				default:
					break;
				}
			}

			//FIM DA CRITICAL SECTION
			if (!ReleaseMutex(hgMutexReadNewMessage))
			{
				//erro lançar mutex
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
		_tprintf(TEXT("Bola x-%d \n"), game->ball.ballPosition.x);
	}

	return 0;
}

VOID freeThreads(ServerThreadsHandlers handlers) 
{
	CloseHandle(handlers.hThreadConsumer);
	CloseHandle(handlers.hThreadBall);
}