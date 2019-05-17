/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include "..\Communicate\stdafx.h"
#pragma warning(disable : 4996)//REMOVE

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

	//loadGameConfiguration(TEXT("config.txt"), &serverConfig);

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

	//	if (!initServerPipeLocal(&*serverInstance.serverHandlers.namedPipeInstances, MAX_PLAYER_INSTANCES))
	//esta a compilar como c++
	if (!initServerPipeLocal(serverInstance.serverHandlers.namedPipeInstances, MAX_PLAYER_INSTANCES))
	{
		_tprintf(TEXT("ERRO Criar Pipes"));
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

	serverInstance.serverHandlers.threadHandlers.hThreadBall = CreateThread(
		NULL,
		0,
		BallThread,	//nome da funçao
		(LPVOID) serverInstance.serverHandlers.sharedMemHandlers.lpSharedMemGame,					//Argumento a ser passado
		0,						//flag de ciração
		&serverInstance.serverHandlers.threadHandlers.dwThreadBall //idThread
	);

	if ( serverInstance.serverHandlers.threadHandlers.hThreadConsumer == NULL)
	{
		exit(EXIT_FAILURE);
	}


	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadBall, INFINITE);


	loadGameConfiguration(TEXT("config.txt"), &serverConfig);

	getTopTenRegistry(scoreTopTen);
	
	int opcao, i;
	ScorePlayer novoScore;

	ZeroMemory(&novoScore, sizeof(novoScore));

	do {
		_tprintf(TEXT("Meu para debug\n\n"));
		_tprintf(TEXT("1- ver configurações lidas do ficheiro.\n"));
		_tprintf(TEXT("2- Mostrar Pontuações lidas no registry.\n"));
		_tprintf(TEXT("3- Inserir nova pontuação.\n"));
		_tprintf(TEXT("4- Sair (guarda novas pontuações no registry).\n"));
		_tscanf_s(TEXT(" %d"), &opcao);

		switch (opcao) {
		case 1:
			_tprintf(TEXT("\nConfiguração\n"));
			_tprintf(TEXT("Niveis: %d\n"), serverConfig.niveis);
			_tprintf(TEXT("SpeedUps: %d\n"), serverConfig.speedUps);
			_tprintf(TEXT("SlowDowns: %d\n"), serverConfig.slowDowns);
			_tprintf(TEXT("Vidas Iniciais: %d\n"), serverConfig.vidasIniciais);
			_tprintf(TEXT("Tijolos Iniciais: %d\n"), serverConfig.tejolosIniciais);
			_tprintf(TEXT("prob speedup: %f\n"), serverConfig.probSpeedUp);
			_tprintf(TEXT("prob slowdown: %f\n"), serverConfig.probSlowDowns);
			_tprintf(TEXT("prob bonus: %f\n"), serverConfig.fBonusProbabilities);
			_tprintf(TEXT("duracao: %d\n"), serverConfig.duracao);
			_tprintf(TEXT("velocidade bola: %f\n\n"), serverConfig.fVelocityBall);
			break;
		case 2:
			_tprintf(TEXT("\nPontuação\n"));
			for (i = 0; i < 10; i++) {
				_tprintf(TEXT("jogador: %s, Pontuação: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
			}
			break;
		case 3:
			_tprintf(TEXT("\nNova pontuação\n"));
			_tprintf(TEXT("Nome Jogador: "));
			_tscanf(TEXT("%s"), novoScore.jogador);
			_tprintf(TEXT("Pontuação: "));
			_tscanf(TEXT("%lf"), &novoScore.pontuacao);
			setScoreTopTen(novoScore, scoreTopTen);
			_tprintf(TEXT("\n"));
			break;
		case 4:
			setTopTenRegistry(scoreTopTen);
			break;
		default:
			break;
		}

	} while (opcao != 4);
	


	_tcscpy_s(novoScore.jogador,_countof(novoScore.jogador) ,TEXT("Luis"));
	novoScore.pontuacao = 21;
	setScoreTopTen(novoScore, scoreTopTen);
	_tprintf(TEXT("Apos novo score\n\n"));
	for (i = 0; i < 10; i++) {
		_tprintf(TEXT("jogador: %s, Pontuação: %.2f\n"), scoreTopTen[i].jogador, scoreTopTen[i].pontuacao);
	}

	//setTopTenRegistry(scoreTopTen);


	freeSyncObject();
	freeMappedMemoryServer(&serverInstance.serverHandlers.sharedMemHandlers);
	freeThreads(serverInstance.serverHandlers.threadHandlers);



	_gettchar();
	return 0;
}