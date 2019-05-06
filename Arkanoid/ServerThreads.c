#include "ServerThreads.h"
#include "Server.h"
#include "GameLogic.h"

static PlayerInfo lastSender; //FIX temporario

DWORD WINAPI ProducerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* writeMessage = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;

	DWORD dwWaitEvent;

	while (1)
	{
		dwWaitEvent = WaitForSingleObject(hgSyncRWObject, INFINITE);
		_tprintf_s(TEXT("\n Vamos escrever"));
		
		//APENAS ESTÁ A RESSPONDER LOGIN
		_tcscpy_s(writeMessage->queueOfMessageServerClient[writeMessage->wFirstUnReadMessageIndexSC].messagePD.tcDestination,
			_countof(writeMessage->queueOfMessageServerClient[writeMessage->wFirstUnReadMessageIndexSC].messagePD.tcDestination),
			lastSender.tcUserName);

		writeMessage->queueOfMessageServerClient[writeMessage->wFirstUnReadMessageIndexSC].response = ResponseLoginSuccess;

		SetEvent(hgReadObject);
		ResetEvent(hgSyncRWObject);
	}
	return 0;
}

DWORD WINAPI ConsumerMessageThread(LPVOID lpArg)
{
	Server* serverObj = (Server*)lpArg;
	MessageQueue* queue = (MessageQueue*)serverObj->serverHandlers.sharedMemHandlers.LpSharedMemMessage;

	DWORD dwWaitEvent;
	ZeroMemory(&lastSender, sizeof(PlayerInfo));

	while (1)
	{
		_tprintf_s(TEXT("\nA espera ..."));
		dwWaitEvent = WaitForSingleObject(hgWriteObject, INFINITE);
		_tprintf_s(TEXT("\n Jogador : %s| Concectou-se %d"),
			serverObj->gameInstance.lobbyGame.playersInLobby[0].tcUserName,
			serverObj->gameInstance.lobbyGame.wPlayersInLobby);
		switch (dwWaitEvent)
		{
		case WAIT_OBJECT_0:

			switch (queue->queueOfMessageClientServer[queue->wFirstUnReadMessageIndex].request)
			{
			case LoginMessage:

				if (addUserNameToLobby(queue->queueOfMessageClientServer[queue->wFirstUnReadMessageIndex].messagePD.tcSender, &serverObj->gameInstance))
				{
					_tcscpy_s(lastSender.tcUserName,
						_countof(lastSender.tcUserName),
						queue->queueOfMessageClientServer[queue->wFirstUnReadMessageIndex].messagePD.tcSender);

					//FIX: Inserir 1 jogador a mais?
					for (size_t i = 0; i < serverObj->gameInstance.lobbyGame.wPlayersInLobby; i++)
					{
						_tprintf_s(TEXT("\n Jogador : %s| Concectou-se %d"),
							serverObj->gameInstance.lobbyGame.playersInLobby[i].tcUserName,
							serverObj->gameInstance.lobbyGame.wPlayersInLobby);
					}
					//VAMOS NOTIFICAR A thread Produtora
					//VER COMO CRITICAL SECTION FUNCIONA!!!
					if (!SetEvent(hgSyncRWObject))
					{
						_tprintf_s(TEXT("\n Erro set evento"));
					}
				}

				break;
			case QuitGameMessage:
				break;
			default:
				break;
			}

			break;
		default:
			_tprintf(TEXT("Erro \n"));
			break;
		}

		ResetEvent(hgWriteObject);

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
	}

	return 0;
}

VOID freeThreads(ServerThreadsHandlers handlers) 
{
	CloseHandle(handlers.hThreadConsumer);
	CloseHandle(handlers.hThreadProducer);
	CloseHandle(handlers.hThreadBall);
}