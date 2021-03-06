#include "Server.h"
#include "..\Communicate\CircularBuffer.h"
#include <sddl.h>


BOOL intitServerGameMem(HANDLE* hMapObj, LPVOID* lpSharedMem)
{
	//Mapear sharedMem para o jogo
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE, //usar paging file
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(Game),
		NAME_SHARED_MEMORY_GAME);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}


	//ja existe uma instancia do servidor a correr
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,					// qual o objeto a mapear
		FILE_MAP_WRITE,				//read/write 
		0,			//HIGH bit
		0,				//Low bit
		0);		//inicio da mem

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}

	//Limpar memoria
	ZeroMemory(*lpSharedMem, sizeof(Game));

	return TRUE;
}

BOOL intitServerMessageMem(HANDLE* hMapObj, LPVOID* lpSharedMem)
{
	*hMapObj = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MessageQueue),
		NAME_SHARED_MEMORY_MESSAGE);

	if (*hMapObj == NULL)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}

	*lpSharedMem = MapViewOfFile(
		*hMapObj,
		FILE_MAP_ALL_ACCESS, //write/read
		0,
		0,
		0);

	if (*lpSharedMem == NULL)
	{
		return FALSE;
	}

	ZeroMemory(*lpSharedMem, sizeof(MessageQueue));

	return TRUE;
}

BOOL initServerPipe(NamedPipeInstance npInstances[], WORD wInstances)
{
	WORD i;
	SECURITY_ATTRIBUTES sa_attributes;

	initSecurityAtributes(&sa_attributes);

	for (i = 0; i < wInstances; i++)
	{
		ZeroMemory(&npInstances[i], sizeof(OVERLAPPED));

		npInstances[i].hMyEvent = CreateEvent(
			NULL,	//security
			TRUE,	//Manual - reset 
			/*Functions such as GetOverlappedResult and the synchronization
			 *wait functions reset auto-reset events to the nonsignaled state. Therefore,
			 *you should use a manual reset event*/
			TRUE,	//estado inicial- unsigned
			NULL);	//sem nome

		if (npInstances[i].hMyEvent == NULL)
		{
			_tprintf(TEXT("\nErro criar evento para overlapped ERRO: %d"), GetLastError());
			return FALSE;
		}
		//TODO MUDAR NOME
		npInstances[i].oOverLapReader.hEvent = npInstances[i].hMyEvent;

		npInstances[i].hNamedPipeReadFromClient = CreateNamedPipe(
			NAME_NAMED_PIPE_WRITE_TO_SERVER,		//Nome do pipe
			PIPE_ACCESS_INBOUND |			//Pipe read
			FILE_FLAG_OVERLAPPED,			//overlapped mode ON
			PIPE_TYPE_MESSAGE |				//Vamos passar uma estrutura
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,						//MODO bloquante
			/*The pipe server should not perform a blocking read operation until the pipe client has started.
			 *Otherwise, a race condition can occur.
			 * This typically occurs when initialization code,
			 * such as the C run-time, needs to lock and examine inherited handles.*/
			wInstances,						//N�mero de named pipes a criar
			sizeof(MessageProtocolPipe),	// Tamanho das mensagens que vai escrever
			sizeof(MessageProtocolPipe),	//Tamanho das mensagens que vai ler
			0,								//TimeOut
			&sa_attributes					//Atributos de seguran�a
		);


		npInstances[i].hNamedPipeWriteToClient = CreateNamedPipe(
			NAME_NAMED_PIPE_READ_FROM_SERVER,		//Nome do pipe
			PIPE_ACCESS_OUTBOUND //|	//Pipe read
			/*			FILE_FLAG_OVERLAPPED*/,	//overlapped mode ON
			PIPE_TYPE_MESSAGE |		//Vamos passar uma estrutura
			PIPE_WAIT,				//MODO bloquante
			/*The pipe server should not perform a blocking read operation until the pipe client has started.
			 *Otherwise, a race condition can occur.
			 * This typically occurs when initialization code,
			 * such as the C run-time, needs to lock and examine inherited handles.*/
			wInstances,					//N�mero de named pipes a criar
			sizeof(MessageProtocolPipe),	// Tamanho das mensagens que vai escrever
			sizeof(MessageProtocolPipe),	//Tamanho das mensagens que vai ler
			0,							//TimeOut
			&sa_attributes							//Atributos de seguran�a
		);

		if (npInstances[i].hNamedPipeReadFromClient == INVALID_HANDLE_VALUE ||
			npInstances[i].hNamedPipeWriteToClient == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("CreateNamedPipe failed with %d.\n"), GetLastError());
			return FALSE;
		}

		npInstances[i].fPendigIO = ConnectNewClientToNP(
			npInstances[i].hNamedPipeReadFromClient,
			&npInstances[i].oOverLapReader);

		npInstances[i].State = npInstances[i].fPendigIO ? ConnectingState :
			ReadState;
	}

	return TRUE;
}

VOID broadCastGameData(NamedPipeInstance* npInst, Server* serverObj)
{
	Game* gameObj = (Game*)serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame;

	MessageProtocolPipe myMessage;

	ZeroMemory(&myMessage, sizeof(MessageProtocolPipe));

	CopyMemory(&myMessage.messagePD.gameData, gameObj, sizeof(Game));

	writeMessageToClientPipeResponse(&myMessage, ResponseGameData, NAME_SERVER, TEXT("*"));

	for (size_t i = 0; i < MAX_PLAYER_INSTANCES; i++)
	{
		if (npInst->State != ConnectingState)
		{
			WriteFile(npInst[i].hNamedPipeWriteToClient,
				&myMessage,
				sizeof(MessageProtocolPipe),
				NULL,
				NULL);
		}
	}
}

VOID writeMessageToClientSharedMemory(MessageQueue* mqArg, TypeOfResponseMessage response, const PTCHAR pSender, const PTCHAR pDestination)
{
	MessageProtocolDatagram aux;
	ZeroMemory(&aux, sizeof(MessageProtocolDatagram));

	fillMessageToProtocolDatagram(
		&aux,
		pSender,
		pDestination);

	switch (response)
	{
	case ResponseTop10:
		ScorePlayer list[NUM_TOP];
		ZeroMemory(&list, sizeof(ScorePlayer) * NUM_TOP);

		getTopTenRegistry(list);
		CopyMemory(&aux.listOfHighScores, //destination
			list,
			sizeof(ScorePlayer) * NUM_TOP);

		break;
	case ResponseLoginFail:
		break;
	case ResponseLoginSuccess:
		break;
	}

	aux.response = response;

	addItemToBuffer(&mqArg->circularBufferServerClient, &aux);
}

VOID writeMessageToClientPipeResponse(MessageProtocolPipe* mppArg, TypeOfResponseMessage response, const PTCHAR pSender, const PTCHAR pDestination)
{
	mppArg->wTypeOfMessage = TYPE_OF_MESSAGE_RESPONSE;

	mppArg->response = response;

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

VOID freeMappedMemoryServer(ServerSharedMemoryHandlers* mapped)
{
	UnmapViewOfFile(mapped->lpSharedMemGame);
	UnmapViewOfFile(mapped->LpSharedMemMessage);

	CloseHandle(mapped->hMapObjGame);
	CloseHandle(mapped->hMapObjMessage);
}

BOOL loadGameConfiguration(TCHAR *nomeFicheiro, GameServerConfiguration *serverConfig)
{
	TCHAR buffer[256];
	TCHAR* word = NULL;
	TCHAR* tcNextToken = NULL;
	FILE *fOpenFile;

	errno_t  file = _tfopen_s(&fOpenFile, nomeFicheiro, TEXT("r"));

	ZeroMemory(&buffer, sizeof(buffer));

	if (file != 0)
	{
		_tprintf(TEXT("Ficheiro n�o existente \n"));
		return FALSE;
	}

	while (_fgetts(buffer, 255, fOpenFile) != NULL)
	{
		if (_tcsstr(buffer, TEXT_FILE_LEVELS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->niveis = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_TIME_TO_WAIT_FOR_PLAYERS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->wTimeToWaitForNewPlayers = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_INITIAL_LIFES) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->vidasIniciais = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_DURATION_BONUS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->duracaoEfeitos = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_PROB_SPEED_UPS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->probSpeedUp = _tstof(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_PROB_SLOW_DOWNS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->probSlowDowns = _tstof(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_PROB_EXTRA_HEALTH) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->probVidaExtra = _tstof(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_PROB_BONUS_TRIPLE) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->probTriple = _tstof(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_BALL_SPEED) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->wVelocityBall = _tstoi(word);
			continue;
		}
	}

	if (fOpenFile)
	{
		if (fclose(fOpenFile) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL setTopTenRegistry(ScorePlayer scoreTopTen[]) {
	HKEY chave;  //handle da chave(n�o � objeto do nucleo )
	DWORD infoState;
	TCHAR jogadores[100];
	TCHAR jogador[250];
	TCHAR pontuacoes[100];
	TCHAR pontuacao[250];
	DWORD valores[100] = { 2,5,25,100 };
	int n_preenchidos = 4;
	int i;
	ZeroMemory(jogadores, sizeof(jogadores));

	ZeroMemory(jogador, sizeof(jogador));
	ZeroMemory(pontuacoes, sizeof(pontuacoes));
	ZeroMemory(pontuacao, sizeof(pontuacao));


	if (scoreTopTen == NULL) {
		return FALSE;
	}

	for (i = 0; i < 10; i++) {

		if (_tcsclen(scoreTopTen[i].jogador) == 0)
			break;


		//_stprintf(jogador, TEXT("%s"), scoreTopTen[i].jogador);
		//_tcscat_s(jogadores, _countof(jogadores), TEXT("Joao\0"));
		_tcscat_s(jogadores, _countof(jogadores), scoreTopTen[i].jogador);


		_stprintf_s(pontuacao, _countof(pontuacao), TEXT("%d"), scoreTopTen[i].pontuacao);
		_tcscat_s(pontuacoes, _countof(pontuacoes), pontuacao);

		if (i < 9)
		{
			_tcscat_s(jogadores, _countof(jogadores), TEXT(";"));
			_tcscat_s(pontuacoes, _countof(pontuacoes), TEXT(";"));
		}
	}
	if (_tcslen(jogadores) > 0) {
		//		_stprintf(jogador, TEXT("%s"), jogadores);


				//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao  
		if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT_REGISTY_PATH, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &infoState) != ERROR_SUCCESS) {
			_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
			return FALSE;
		}
		else {  //chave criada ou aberta
			//Se a chave foi criada, inicializar os valores    
			if (infoState == REG_CREATED_NEW_KEY || infoState == REG_OPENED_EXISTING_KEY) {
				_tprintf(TEXT("Chave: HKEY_CURRENT_USER\\Software\\Arkanoid criada\n"));
				//Criar valor "Autor" = "Meu nome"  


				RegSetValueEx(chave, TEXT("Jogador"), 0, REG_SZ, (LPBYTE)jogadores, sizeof(jogadores));
				RegSetValueEx(chave, TEXT("Pontuacao"), 0, REG_SZ, (LPBYTE)pontuacoes, sizeof(pontuacoes));
			}
		}
		RegCloseKey(chave);
		return TRUE;
	}
	return FALSE;
}

VOID setScoreTopTen(ScorePlayer newScore, ScorePlayer scoreTopTen[]) {
	int i = 0, j;

	while (newScore.pontuacao < scoreTopTen[i].pontuacao && i < 10) {
		i++;
	}

	for (j = 9; j > i; j--) {

		_tcscpy_s(scoreTopTen[j].jogador,
			_countof(scoreTopTen[j].jogador),
			scoreTopTen[j - 1].jogador);
		scoreTopTen[j].pontuacao = scoreTopTen[j - 1].pontuacao;
	}
	if (i <= 9) {

		_tcscpy_s(scoreTopTen[i].jogador,
			_countof(scoreTopTen[i].jogador),
			newScore.jogador);

		scoreTopTen[i].pontuacao = newScore.pontuacao;
	}
}

BOOL getTopTenRegistry(ScorePlayer scoreTopTen[]) {
	HKEY chave;  //handle da chave(n�o � objeto do nucleo )
	DWORD infoState, tamanho;
	TCHAR jogadores[200];
	TCHAR scores[200];
	TCHAR* jogador;
	TCHAR* score;
	TCHAR* tcNextToken = NULL;
	int i = 0;

	ZeroMemory(jogadores, sizeof(jogadores));
	ZeroMemory(scores, sizeof(scores));

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao  
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT_REGISTY_PATH, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &infoState) != ERROR_SUCCESS)
	{
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		return FALSE;
	}
	else
	{  //chave criada ou aberta
		//Se a chave foi criada, inicializar os valores    
		if (infoState == REG_OPENED_EXISTING_KEY) {

			tamanho = 100 * sizeof(TCHAR);
			RegQueryValueEx(chave, TEXT("Jogador"), NULL, NULL, (LPBYTE)jogadores, &tamanho);
			RegQueryValueEx(chave, TEXT("Pontuacao"), NULL, NULL, (LPBYTE)scores, &tamanho);



			jogador = _tcstok_s(jogadores, TEXT(";"), &tcNextToken);

			for (i = 0; i < 10; i++)
			{
				if (jogador == NULL)
					break;
				else
					_tcscpy_s(scoreTopTen[i].jogador,
						_countof(scoreTopTen[i].jogador),
						jogador);


				// Get next token:
				jogador = _tcstok_s(NULL, TEXT(";"), &tcNextToken);
			}

			tcNextToken = NULL;

			score = _tcstok_s(scores, TEXT(";"), &tcNextToken);

			// Note: strtok is deprecated; consider using strtok_s instead
			for (i = 0; i < 10; i++)
			{
				if (score == NULL)
					break;
				else
					scoreTopTen[i].pontuacao = _tstoi(score);

				// Get next token:
				score = _tcstok_s(NULL, TEXT(";"), &tcNextToken); // C4996
			}
		}
	}

	RegCloseKey(chave);
	return TRUE;

}

static BOOL checkUserNameInLobby(PTCHAR userName, const ServerGameInstance* gameArg)
{
	for (size_t i = 0; i < gameArg->lobbyGame.wPlayersInLobby; i++)
	{
		if (_tcscmp(gameArg->lobbyGame.playersInLobby[i].tcUserName, userName) == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL checkPlayerInGame(const PTCHAR userName, const ServerGameInstance* players)
{
	if (players->GameStates == GameInProgress)
	{
		for (size_t i = 0; i < players->usersInGame.wPlayersPlaying; i++)
		{
			if (_tcscmp(players->usersInGame.playersPlaying[i].tcUserName, userName) == 0)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

//TODO: VER ALGUM PLAYER EM JOGO QUE SE QUUER CONNECTAR
BOOL addUserNameToLobby(PTCHAR userName, Server* server)
{
	ServerGameInstance * gameLobby = (ServerGameInstance*)&server->gameInstance;
	if (checkUserNameInLobby(userName, gameLobby) && checkPlayerInGame(userName, &server->gameInstance))
	{
		_tcscpy_s(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby++].tcUserName,	//destino
			_countof(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby].tcUserName),	//tamanho que o destino suporta
			userName		//Origem
		);

		startGame(server);

		return TRUE;
	}
	return FALSE;
}

VOID addUsersToGame(Server* serverObj)
{
	UsersPlaying* playersInGame = (UsersPlaying*)&serverObj->gameInstance.usersInGame;

	WORD wWidthPaddle = 70;

	for (size_t i = 0; i < serverObj->gameInstance.lobbyGame.wPlayersInLobby; i++)
	{
		_tcscpy_s(playersInGame->playersPlaying[i].tcUserName,	//destino
			_countof(playersInGame->playersPlaying[i].tcUserName),	//tamanho que o destino suporta
			serverObj->gameInstance.lobbyGame.playersInLobby[i].tcUserName		//Origem
		);

		CopyMemory(&playersInGame->playersPlaying[i], //destino
			&serverObj->gameInstance.lobbyGame.playersInLobby[i], //origem
			sizeof(PlayerInfo));		//Tamanho



		createPlayerPaddle(
			playersInGame->playersPlaying[i].tcUserName, //owner
			(Game*)serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame); //ponteiro

		ZeroMemory(&serverObj->gameInstance.lobbyGame.playersInLobby[i], sizeof(PlayerInfo));
	}
	playersInGame->wPlayersPlaying = serverObj->gameInstance.lobbyGame.wPlayersInLobby;
	serverObj->gameInstance.lobbyGame.wPlayersInLobby = 0;

}

VOID transferPlayersToGame(Server* serverObj)
{
	addUsersToGame(serverObj);
	serverObj->gameInstance.GameStates = GameInProgress;
}

VOID startGame(Server* serverObj)
{
	Game* game = (Game*)serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame;

	setHealth(game, serverConfig.vidasIniciais);

	//Vamos inicializar a thread da bola
	ResumeThread(serverObj->serverHandlers.threadHandlers.hThreadBall);
	ResumeThread(serverObj->serverHandlers.threadHandlers.hThreadBonus);
}

WORD getPlayersInLobby(const Lobby* lobby)
{
	return lobby->wPlayersInLobby;
}

VOID tryToMovePaddle(const PTCHAR username, Server* serverObj, const short nSideToMove)
{
	if (serverObj->gameInstance.GameStates != GameInProgress)
	{
		_tprintf(TEXT("\nComportamento nao esperado NENHUM JOGO A CORRER1!!"));
		return;
	}
	else
	{
		movePaddle(username, (Game*)serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame, nSideToMove);
	}
}

VOID initSecurityAtributes(SECURITY_ATTRIBUTES* saArg)
{
	const TCHAR* szSD = TEXT("D:")
		TEXT("(A;OICI;GA;;;BG)")
		TEXT("(A;OICI;GA;;;AN)")
		TEXT("(A;OICI;GA;;;AU)")
		TEXT("(A;OICI;GA;;;BA)");

	saArg->nLength = sizeof(SECURITY_ATTRIBUTES);
	saArg->bInheritHandle = FALSE;


	ConvertStringSecurityDescriptorToSecurityDescriptor(szSD,
		SDDL_REVISION_1,
		&(saArg->lpSecurityDescriptor),
		NULL);
}

BOOL ConnectNewClientToNP(HANDLE hNamedipe, LPOVERLAPPED lpo)
{
	BOOL bPendingIO = FALSE;

	if (ConnectNamedPipe(hNamedipe, lpo))
	{
		_tprintf(TEXT("ConnectNamedPipe failed with %d.\n"), GetLastError());
		return FALSE;
	}

	switch (GetLastError())
	{
		//Esta a decorrer um a�ao assincrona
	case ERROR_IO_PENDING:
		bPendingIO = TRUE;
		break;
		//Um cliente conectou-se dentro do time-out 
	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;
	default:
		//nao estavamos a espera 
		_tprintf(TEXT("\nErro no connect named pipe [%d]"), GetLastError());
		return  FALSE;
	}
	return bPendingIO;
}

VOID DisconnectAndReconnect(NamedPipeInstance* npToDisconect)
{
	if (!DisconnectNamedPipe(npToDisconect->hNamedPipeReadFromClient) ||
		!DisconnectNamedPipe(npToDisconect->hNamedPipeWriteToClient))
	{
		_tprintf(TEXT("\nDiscone��o do named pipe falhou com o erro %d"), GetLastError());
	}

	npToDisconect->fPendigIO = ConnectNewClientToNP(
		npToDisconect->hNamedPipeReadFromClient,
		&npToDisconect->oOverLapReader);

	npToDisconect->State = npToDisconect->fPendigIO ?
		ConnectingState :
		ReadState;
}

VOID removePlayer(Server* serverObj, const PTCHAR userNameToRemove)
{
	if (serverObj->gameInstance.GameStates == WaitingForPlayers)
	{
		removePlayerFromLobby(&serverObj->gameInstance.lobbyGame, userNameToRemove);
	}
	else
	{
		if (!removePlayerFromGame(&serverObj->gameInstance.usersInGame, serverObj->serverHandlers.sharedMemHandlers.lpSharedMemGame, userNameToRemove))
		{
			//jogo pode esstar a decorrer mas existe pessoas a "ver"
			if (removePlayerFromLobby(&serverObj->gameInstance.lobbyGame, userNameToRemove))
			{
				_tprintf(TEXT("\n User : %s Removido do lobby!!"), userNameToRemove);
			}
		}
		else
		{
			_tprintf(TEXT("\n User : %s Removido do jogo!!"), userNameToRemove);
		}
	}
}

BOOL removePlayerFromLobby(Lobby* lobbyPtr, const PTCHAR userNameToRemove)
{
	for (WORD i = 0; i < lobbyPtr->wPlayersInLobby; i++)
	{
		if (_tcscmp(userNameToRemove, lobbyPtr->playersInLobby[i].tcUserName) == 0)
		{
			ZeroMemory(&lobbyPtr->playersInLobby[i], sizeof(PlayerInfo));

			for (WORD j = i; j + 1 < lobbyPtr->wPlayersInLobby; j++)
			{
				CopyMemory(&lobbyPtr->playersInLobby[j],
					&lobbyPtr->playersInLobby[j + 1],
					sizeof(PlayerInfo));

				ZeroMemory(&lobbyPtr->playersInLobby[j + 1], sizeof(PlayerInfo));
			}
			lobbyPtr->wPlayersInLobby--;
			return FALSE;
		}
	}
	return  FALSE;
}

BOOL removePlayerFromGame(UsersPlaying* userInGamePtr, Game* gameObj, const PTCHAR userNameToRemove)
{
	for (WORD i = 0; i < userInGamePtr->wPlayersPlaying; i++)
	{
		if (_tcscmp(userNameToRemove, userInGamePtr->playersPlaying[i].tcUserName) == 0)
		{
			ZeroMemory(&gameObj->PlayerPaddles[i], sizeof(PlayerInfo));
			for (size_t j = i; j + 1 < gameObj->wNumberOfPlayerPaddles; j++)
			{
				CopyMemory(&userInGamePtr->playersPlaying[j],
					&userInGamePtr->playersPlaying[j + 1],
					sizeof(Paddle));

				ZeroMemory(&userInGamePtr->playersPlaying[j + 1], sizeof(PlayerInfo));
			}
			destroyPlayerPaddle(userNameToRemove, gameObj);
			userInGamePtr->wPlayersPlaying--;
			return  TRUE;
		}
	}
	return FALSE;
}
