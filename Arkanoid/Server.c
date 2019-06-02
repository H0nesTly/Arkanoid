#include "Server.h"
#include "../Communicate/CircularBuffer.h"

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

BOOL initServerPipeLocal(NamedPipeInstance npInstances[], WORD wInstances)
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
			wInstances,						//Número de named pipes a criar
			sizeof(MessageProtocolPipe),	// Tamanho das mensagens que vai escrever
			sizeof(MessageProtocolPipe),	//Tamanho das mensagens que vai ler
			0,								//TimeOut
			NULL					//Atributos de segurança
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
			wInstances,					//Número de named pipes a criar
			sizeof(MessageProtocolPipe),	// Tamanho das mensagens que vai escrever
			sizeof(MessageProtocolPipe),	//Tamanho das mensagens que vai ler
			0,							//TimeOut
			NULL							//Atributos de segurança
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
		_tprintf(TEXT("Ficheiro não existente \n"));
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
		if (_tcsstr(buffer, TEXT_FILE_SPEED_UPS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->speedUps = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_SLOW_DOWNS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->slowDowns = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_INITIAL_LIFES) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->vidasIniciais = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_INITIAL_BLOCKS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->tejolosIniciais = _tstoi(word);
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
		if (_tcsstr(buffer, TEXT_FILE_PROB_BONUS) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->fBonusProbabilities = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_DURATION_LEVEL) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->duracao = _tstoi(word);
			continue;
		}
		if (_tcsstr(buffer, TEXT_FILE_BALL_SPEED) != NULL)
		{
			word = _tcstok_s(buffer, TEXT(": "), &tcNextToken);
			word = _tcstok_s(NULL, TEXT(": "), &tcNextToken);
			serverConfig->fVelocityBall = _tstof(word);
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
	HKEY chave;  //handle da chave(não é objeto do nucleo )
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


		_stprintf_s(pontuacao, _countof(pontuacao), TEXT("%.2f"), scoreTopTen[i].pontuacao);
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
		return TRUE;
		RegCloseKey(chave);
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
	HKEY chave;  //handle da chave(não é objeto do nucleo )
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

			//jogador = _tcstok_s(jogadores, TEXT(","), &tcNextToken);


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
					scoreTopTen[i].pontuacao = _tstof(score);

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

//Vamos criar os blocos
//TODO: CRIAR BLOCOS COM ALTURAS E COORDENADAS
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

		createPlayerPaddle(200, DEFAULT_POS_Y_PLAYER_PADDLE, //coords
			HEIGTH_OF_PLAYER_PADDLE, wWidthPaddle,  //dimensoes
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
	//Vamos inicializar a thread da bola
	ResumeThread(serverObj->serverHandlers.threadHandlers.hThreadBall);
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
	PSECURITY_ATTRIBUTES pSD;
	PACL pACL;
	EXPLICIT_ACCESS ea;
	PSID pEveryonesSID = NULL, pAdminSD = NULL;
	SID_IDENTIFIER_AUTHORITY sidAuthWorld = SECURITY_WORLD_SID_AUTHORITY; // represents the top-level authority of a security identifier (SID).
	TCHAR str[256];

	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);

	if (pSD == NULL)
	{
		_tprintf(TEXT("Erro LocalAlloc!!!"));
		return;
	}
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
	{
		_tprintf(TEXT("Erro IniSec!!!"));
		return;
	}

	// Create a well-known SID for the Everyone group.
	if (!AllocateAndInitializeSid(
		&sidAuthWorld,			//This structure provides the top-level identifier authority value to set in the SID.
		1,						//numero de sub authority
		SECURITY_WORLD_RID,		//
		0, 0, 0, 0, 0, 0, 0,	//sem sub authority
		&pEveryonesSID))		//A pointer to a variable that receives the pointer to the allocated and initialized SID structure.
	{
		_stprintf_s(str, 256, TEXT("AllocateAndInitializeSid() error %u"), GetLastError());
		_tprintf(str);
		Cleanup(pEveryonesSID, pAdminSD, NULL, pSD);
	}
	else
	{
		_tprintf(TEXT("AllocateAndInitializeSid() for the Everyone group is OK"));
	}

	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));

	ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea.Trustee.ptstrName = (LPTSTR)pEveryonesSID;

	if (SetEntriesInAcl(1, &ea, NULL, &pACL) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro SetAcl!!!"));
		return;
	}

	if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE)) {
		_tprintf(TEXT("Erro IniSec!!!"));
		return;
	}

	saArg->nLength = sizeof(*saArg);
	saArg->lpSecurityDescriptor = pSD;
	saArg->bInheritHandle = TRUE;
}

// Buffer clean up routine
void Cleanup(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD)
{
	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
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
		//Esta a decorrer um açao assincrona
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
		_tprintf(TEXT("\nDisconeção do named pipe falhou com o erro %d"), GetLastError());
	}

	npToDisconect->fPendigIO = ConnectNewClientToNP(
		npToDisconect->hNamedPipeReadFromClient,
		&npToDisconect->oOverLapReader);

	npToDisconect->State = npToDisconect->fPendigIO ?
		ConnectingState :
		ReadState;
}
