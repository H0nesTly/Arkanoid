#include "Server.h"
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


BOOL intitServerGameMem(HANDLE* hMapObj, LPVOID* lpSharedMem )
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

	HANDLE file = _tfopen(nomeFicheiro, "r");
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

BOOL setTopTenRegestry(ScorePlayer scoreTopTen[]) {
	HKEY chave;  //handle da chave(não é objeto do nucleo )
	DWORD queAconteceu;
	TCHAR jogadores[250];

	ZeroMemory(jogadores, sizeof(jogadores));
	int i, j, k;

	if (scoreTopTen == NULL) {
		return FALSE;
	}

	/********************Problema aqui!!!!!!***********************/
	for (i = 0; i < 10; i++) {
		_tcscat_s(jogadores, _countof(jogadores), scoreTopTen[i].jogador);
		_tcscat_s(jogadores, _countof(jogadores), TEXT(","));

	}
	/*************************************************************/

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao  
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Arkanoid"), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &queAconteceu) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		return FALSE;
	}
	else {  //chave criada ou aberta
		//Se a chave foi criada, inicializar os valores    
		if (queAconteceu == REG_CREATED_NEW_KEY || queAconteceu == REG_OPENED_EXISTING_KEY) {
			_tprintf(TEXT("Chave: HKEY_CURRENT_USER\\Software\\Arkanoid criada\n"));
			//Criar valor "Autor" = "Meu nome"  
			RegSetValueEx(chave, TEXT("Jogador"), 0, REG_SZ, (LPBYTE)TEXT("João Santos,Diogo Pires,Francisco,Miguel,Rita,Maria,Mariana,Sandra,Filipe,Gonçalo"), _tcslen(TEXT("João Santos,Diogo Pires,Francisco,Miguel,Rita,Maria,Mariana,Sandra,Filipe,Gonçalo")) * sizeof(TCHAR));
			RegSetValueEx(chave, TEXT("Pontuacao"), 0, REG_SZ, (LPBYTE)TEXT("20,20,19,18,17,17,15,13,13,5"), _tcslen(TEXT("20,20,19,18,17,17,15,13,13,5")) * sizeof(TCHAR));
		}
	}
	return TRUE;
	RegCloseKey(chave);
}

VOID setScoreTopTen(ScorePlayer newScore, ScorePlayer scoreTopTen[]) {
	int i=0, j;

	while (newScore.pontuacao < scoreTopTen[i].pontuacao && i < 10) {
		i++;
	}

	for (j = 9; j > i; j--) {
		_tcscpy(scoreTopTen[j].jogador, scoreTopTen[j-1].jogador);
		scoreTopTen[j].pontuacao = scoreTopTen[j-1].pontuacao;
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
			
			
			jogador = _tcstok(jogadores, TEXT(","));
			
			// Note: strtok is deprecated; consider using strtok_s instead
			for (i = 0; i < 10; i++)
			{
				if (jogador == NULL)
					_tcscpy(scoreTopTen[i].jogador, TEXT(""));
				else
					_tcscpy(scoreTopTen[i].jogador, jogador);


				// Get next token:
				jogador = _tcstok(NULL, TEXT(",")); // C4996
			}

			score = _tcstok(scores, TEXT(","));

			// Note: strtok is deprecated; consider using strtok_s instead
			for (i = 0; i < 10; i++)
			{
				if (score == NULL)
					scoreTopTen[i].pontuacao = 0;
				else
					scoreTopTen[i].pontuacao = _tstof(score);

				// Get next token:
				score = _tcstok(NULL, TEXT(",")); // C4996
			}
		}
	}
	
	RegCloseKey(chave);
	return TRUE;

}

BOOL verifyUserName(PTCHAR userName)
{
	return 0;

}