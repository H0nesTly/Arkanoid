#include "ServerThreads.h"
#include "Server.h"

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

	/*while (1)
	{
		_tprintf_s(TEXT("\nA espera ..."));
		dwWaitEvent = WaitForSingleObject(hgWriteObject, INFINITE);

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

	}*/
	return 0;

}
