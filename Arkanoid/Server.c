#include "Server.h"
#pragma warning(disable : 4996)

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

VOID freeMappedMemory(ServerSharedMemoryHandlers* mapped)
{
	UnmapViewOfFile(mapped->lpSharedMemGame);
	UnmapViewOfFile(mapped->LpSharedMemMessage);

	CloseHandle(mapped->hMapObjGame);
	CloseHandle(mapped->hMapObjMessage);
}

BOOL leituraFicheiroConfig(TCHAR *nomeFicheiro, GameServerConfiguration *serverConfig)
{
	TCHAR buffer[256];
	TCHAR* word = NULL;


	ZeroMemory(&buffer, sizeof(buffer));

	HANDLE file = _tfopen(nomeFicheiro, "r");

	HANDLE file = _tfopen(nomeFicheiro, TEXT("r"));

	if (file == NULL) {
		_tprintf(TEXT("Ficheiro não existente \n"));
		return FALSE;
	}

	while (_fgetts(buffer, 255, file) != NULL) {
		if (_tcsstr(buffer, TEXT("niveis")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->niveis = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("speedups")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->speedUps = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("slowdowns")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->slowDowns = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("vidasiniciais")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->vidasIniciais = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("tijolosiniciais")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->tejolosIniciais = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("probabilidadespeedup")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->probSpeedUp = _tstof(word);
		}
		if (_tcsstr(buffer, TEXT("probabilidadeslowdown")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->probSlowDowns = _tstof(word);
		}
		if (_tcsstr(buffer, TEXT("probabilidadebonus")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->fBonusProbabilities = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("duracao")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->duracao = _tstoi(word);
		}
		if (_tcsstr(buffer, TEXT("velocidadebola")) != NULL) {
			word = _tcstok(buffer, TEXT(": "));
			word = _tcstok(NULL, TEXT(": "));
			serverConfig->fVelocityBall = _tstof(word);
		}

	}

	return TRUE;
}

BOOL setTopTenRegistry(ScorePlayer scoreTopTen[]) {
	HKEY chave;  //handle da chave(não é objeto do nucleo )
	DWORD infoState;
	TCHAR jogadores[100];
	TCHAR jogador[250];
	TCHAR pontuacoes[100];
	TCHAR pontuacao[250];
	DWORD versao, tamanho, valores[100] = { 2,5,25,100 };
	int n_preenchidos = 4;
	int i;
	ZeroMemory(jogadores, sizeof(jogadores));

	ZeroMemory(jogador, sizeof(jogador));
	ZeroMemory(pontuacoes, sizeof(pontuacoes));
	ZeroMemory(pontuacao, sizeof(pontuacao));
	

	int i;

	if (scoreTopTen == NULL) {
		return FALSE;
	}

	for (i = 0; i < 10; i++) {
		
		if(strlen(scoreTopTen[i].jogador) == 0)
			break;
		

		
		//_stprintf(jogador, TEXT("%s"), scoreTopTen[i].jogador);
		//_tcscat_s(jogadores, _countof(jogadores), TEXT("Joao\0"));
		_tcscat_s(jogadores, _countof(jogadores), scoreTopTen[i].jogador);
		

		_stprintf(pontuacao, TEXT("%.2f"), scoreTopTen[i].pontuacao);
		_tcscat_s(pontuacoes, _countof(pontuacoes), pontuacao);

		if (i < 9) {
			_tcscat_s(jogadores, _countof(jogadores), TEXT(";"));
			_tcscat_s(pontuacoes, _countof(pontuacoes), TEXT(";"));
		}
	}
	if (strlen(jogadores) > 0) {
//		_stprintf(jogador, TEXT("%s"), jogadores);


		//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao  
		if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Arkanoid"), 0, NULL,
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
		_tcscpy(scoreTopTen[j].jogador, scoreTopTen[j - 1].jogador);
		scoreTopTen[j].pontuacao = scoreTopTen[j - 1].pontuacao;
	}
	if (i <= 9) {
		_tcscpy(scoreTopTen[i].jogador, newScore.jogador);
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
	int i = 0;

	ZeroMemory(jogadores, sizeof(jogadores));
	ZeroMemory(scores, sizeof(scores));

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao  
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Arkanoid"), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &infoState) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		return FALSE;
	}
	else {  //chave criada ou aberta
		//Se a chave foi criada, inicializar os valores    
		if (infoState == REG_OPENED_EXISTING_KEY) {

			tamanho = 100 * sizeof(TCHAR);
			RegQueryValueEx(chave, TEXT("Jogador"), NULL, NULL, (LPBYTE)jogadores, &tamanho);
			RegQueryValueEx(chave, TEXT("Pontuacao"), NULL, NULL, (LPBYTE)scores, &tamanho);

			
			
			jogador = _tcstok(jogadores, TEXT(";"));
			



			jogador = _tcstok(jogadores, TEXT(","));


			// Note: strtok is deprecated; consider using strtok_s instead
			for (i = 0; i < 10; i++)
			{
				if (jogador == NULL)
					break;
				else
					_tcscpy(scoreTopTen[i].jogador, jogador);


				// Get next token:
				jogador = _tcstok(NULL, TEXT(";")); // C4996
			}

			score = _tcstok(scores, TEXT(";"));

			// Note: strtok is deprecated; consider using strtok_s instead
			for (i = 0; i < 10; i++)
			{
				if (score == NULL)
					break;
				else
					scoreTopTen[i].pontuacao = _tstof(score);

				// Get next token:
				score = _tcstok(NULL, TEXT(";")); // C4996
			}
		}
	}

	RegCloseKey(chave);
	return TRUE;

}


static BOOL checkUserNameInLobby(PTCHAR userName, const ServerGameInstance* gameArg)
{
	//Vamos ver se nome no lobby
	if (gameArg->GameStates == WaitingForPlayers)
	{
		for (size_t i = 0; i < gameArg->lobbyGame.wPlayersInLobby; i++)
		{
			if (_tcscmp(gameArg->lobbyGame.playersInLobby[i].tcUserName, userName) == 0)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

//TODO: VER A MEMORIA PARTILHADA SE ALGUM PLAYER EM JOGO
BOOL addUserNameToLobby(PTCHAR userName, ServerGameInstance* gameLobby)
{
	if (checkUserNameInLobby(userName, gameLobby))
	{
		_tcscpy_s(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby++].tcUserName,	//destino
			_countof(gameLobby->lobbyGame.playersInLobby[gameLobby->lobbyGame.wPlayersInLobby].tcUserName),	//tamanho que o destino suporta
			userName		//Origem
		);

		return TRUE;
	}
	return FALSE;
}