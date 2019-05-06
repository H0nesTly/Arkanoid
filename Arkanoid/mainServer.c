/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include <windows.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>


#include "Server.h"
#include "ServerStructures.h"
#include "ServerThreads.h"
#include "ServerSyncObj.h"

int _tmain(int argc, LPTSTR argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	Server serverInstance;
	ScorePlayer scoreTopTen[10];
	GameServerConfiguration serverConfig;

	ZeroMemory(&serverInstance, sizeof(Server));
	ZeroMemory(&scoreTopTen, sizeof(scoreTopTen));
	ZeroMemory(&serverConfig, sizeof(GameServerConfiguration));

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	leituraFicheiroConfig(TEXT("config.txt"), &serverConfig);

	if (!intitServerGameMem(&serverInstance.serverHandlers.sharedMemHandlers.hMapObjGame,
		&serverInstance.serverHandlers.sharedMemHandlers.lpSharedMemGame))
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr!"));
		exit(EXIT_FAILURE);
	}

	if (!intitServerMessageMem(&serverInstance.serverHandlers.sharedMemHandlers.hMapObjMessage,
		&serverInstance.serverHandlers.sharedMemHandlers.LpSharedMemMessage) )
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr"));
		exit(EXIT_FAILURE);
	}

	if(!initSyncObject())
	{
		_tprintf(TEXT("ERRO Criar objetos de sincronização"));
		exit(EXIT_FAILURE);
	}


	serverInstance.serverHandlers.threadHandlers.hThreadConsumer = CreateThread(
		NULL,
		0,
		ConsumerMessageThread,	//nome da funçao
		(LPVOID) &serverInstance,					//Argumento a ser passado
		0,						//Flags de criaçao
		&serverInstance.serverHandlers.threadHandlers.dwIdConsumer //idThread
	);

	serverInstance.serverHandlers.threadHandlers.hThreadProducer = CreateThread(
		NULL,
		0,
		ProducerMessageThread,	//nome da funçao
		(LPVOID)&serverInstance,					//Argumento a ser passado
		0,						//Flags de criaçao(CREATE_SUSPENDED) -> so começar quando existir algum cliente ligado
		&serverInstance.serverHandlers.threadHandlers.dwIdProducer //idThread
	);

	serverInstance.serverHandlers.threadHandlers.hThreadBall = CreateThread(
		NULL,
		0,
		BallThread,	//nome da funçao
		(LPVOID) serverInstance.serverHandlers.sharedMemHandlers.lpSharedMemGame,					//Argumento a ser passado
		0,						//flag de ciração
		&serverInstance.serverHandlers.threadHandlers.dwThreadBall //idThread
	);

	if (serverInstance.serverHandlers.threadHandlers.hThreadProducer == NULL ||
		serverInstance.serverHandlers.threadHandlers.hThreadConsumer == NULL)
	{
		exit(EXIT_FAILURE);
	}


	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadProducer, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadBall, INFINITE);


	//getTopTenRegistry(scoreTopTen);

	//int i;
	//for (i = 0; i < 10; i++) {
	//	_tprintf(TEXT("jogador: %s, Pontuação: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
	//}
	//ScorePlayer novoScore;

	//_tcscpy_s(novoScore.jogador,_countof(novoScore.jogador) ,TEXT("Luis"));
	//novoScore.pontuacao = 21;
	//setScoreTopTen(novoScore, scoreTopTen);
	//_tprintf(TEXT("Apos novo score\n\n"));
	//for (i = 0; i < 10; i++) {
	//	_tprintf(TEXT("jogador: %s, Pontuação: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
	//}

	//setTopTenRegestry(scoreTopTen);


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


	freeSyncObject();
	freeMappedMemory(&serverInstance.serverHandlers.sharedMemHandlers);
	freeThreads(serverInstance.serverHandlers.threadHandlers);


	_gettchar();
	return 0;
}