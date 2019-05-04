/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE EST� ATIVO A
		OP�OA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Server.h"
#include "ServerStructures.h"
#include "ServerThreads.h"

int _tmain(int argc, LPTSTR argv[])
{
	ScorePlayer scoreTopTen[10];
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	ServerHandlers handlers;

	ZeroMemory(&handlers, sizeof(ServerHandlers));

	//UNICODE: Por defeito, a consola Windows n�o processa caracteres wide.
	//A maneira mais f�cil para ter esta funcionalidade � chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	handlers.threadHandlers.hThreadConsumer = CreateThread(
		NULL,
		0,
		ConsumerMessageThread,	//nome da fun�ao
		NULL,					//Argumento a ser passado
		0,						//Flags de cria�ao
		&handlers.threadHandlers.dwIdConsumer //idThread
	);

	handlers.threadHandlers.hThreadProducer = CreateThread(
		NULL,
		0,
		ProducerMessageThread,	//nome da fun�ao
		NULL,					//Argumento a ser passado
		0,						//Flags de cria�ao(CREATE_SUSPENDED) -> so come�ar quando existir algum cliente ligado
		&handlers.threadHandlers.dwIdProducer //idThread
	);

	if (handlers.threadHandlers.hThreadProducer == NULL || 
		handlers.threadHandlers.hThreadConsumer == NULL)
	{
		exit(EXIT_FAILURE);
	}

	//	if (intitServerGameMem(handlers.sharedMemHandlers.hMapObjGame,
	//		handlers.sharedMemHandlers.lpSharedMemGame) == FALSE)
	//	{
	//		_tprintf(TEXT("ERRO Instancia Servidor ja a correr!"));
	//		exit(EXIT_FAILURE);
	//	}


	//if (intitServerMessageMemReader(serverMappedMemory.hMapObjMessage,
	//	serverMappedMemory.LpSharedMemMessage) == FALSE)
	//{
	//	_tprintf(TEXT("ERRO Instancia Servidor ja a correr"));
	//	exit(EXIT_FAILURE);
	//}

	GameServerConfiguration serverConfig;

	leituraFicheiroConfig(TEXT("config.txt"), &serverConfig);

	getTopTenRegistry(scoreTopTen);
	int i;
	for (i = 0; i < 10; i++) {
		_tprintf(TEXT("jogador: %s, Pontua��o: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
	}
	ScorePlayer novoScore;
	_tcscpy(novoScore.jogador, TEXT("Luis"));
	novoScore.pontuacao = 21;
	setScoreTopTen(novoScore, scoreTopTen);
	_tprintf(TEXT("Apos novo score\n\n"));
	for (i = 0; i < 10; i++) {
		_tprintf(TEXT("jogador: %s, Pontua��o: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
	}

	setTopTenRegestry(scoreTopTen);


	/*_tprintf(TEXT("Niveis: %d\n"), serverConfig.niveis);
	_tprintf(TEXT("SpeedUps: %d\n"), serverConfig.speedUps);
	_tprintf(TEXT("SlowDowns: %d\n"), serverConfig.slowDowns);
	_tprintf(TEXT("Vidas Iniciais: %d\n"), serverConfig.vidasIniciais);
	_tprintf(TEXT("Tijolos Iniciais: %d\n"), serverConfig.tejolosIniciais);
	_tprintf(TEXT("prob speedup: %f\n"), serverConfig.probSpeedUp);
	_tprintf(TEXT("prob slowdown: %f\n"), serverConfig.probSlowDowns);
	_tprintf(TEXT("prob bonus: %f\n"), serverConfig.fBonusProbabilities);
	_tprintf(TEXT("duracao: %d\n"), serverConfig.duracao);
	_tprintf(TEXT("velocidade bola: %f\n"), serverConfig.fVelocityBall);*/




	_gettchar();
	return 0;
}