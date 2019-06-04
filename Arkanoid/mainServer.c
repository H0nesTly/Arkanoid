/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include "..\Communicate\stdafx.h"

#include "Server.h"
#include "ServerStructures.h"
#include "ServerThreads.h"
#include "ServerSyncObj.h"

GameServerConfiguration serverConfig;

int _tmain(int argc, LPTSTR argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	Server serverInstance;
	ScorePlayer scoreTopTen[10];

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
		&serverInstance.serverHandlers.sharedMemHandlers.LpSharedMemMessage))
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr"));
		exit(EXIT_FAILURE);
	}

	if (!initSyncObject())
	{
		_tprintf(TEXT("ERRO Criar objetos de sincronização"));
		exit(EXIT_FAILURE);
	}

	if (!initServerPipeLocal(serverInstance.serverHandlers.namedPipeInstances, MAX_PLAYER_INSTANCES))
	{
		_tprintf(TEXT("ERRO Criar Pipes"));
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
	{
		_tprintf(TEXT("\nFicheiro %s carregado %s"), argv[1],
			loadGameConfiguration(argv[1], &serverConfig) ? TEXT("Sucesso") : TEXT("Falhou"));
	}


	serverInstance.serverHandlers.threadHandlers.hThreadConsumer = CreateThread(
		NULL,
		0,
		ConsumerMessageThread,	//nome da funçao
		(LPVOID)&serverInstance,					//Argumento a ser passado
		0,						//Flags de criaçao
		&serverInstance.serverHandlers.threadHandlers.dwIdConsumer //idThread
	);

	serverInstance.serverHandlers.threadHandlers.hThreadBall = CreateThread(
		NULL,
		0,
		BallThread,	//nome da funçao
		(LPVOID)&serverInstance,					//Argumento a ser passado
		CREATE_SUSPENDED,						//flag de criação - SUSPENDIDA
		&serverInstance.serverHandlers.threadHandlers.dwThreadBall //idThread
	);

	serverInstance.serverHandlers.threadHandlers.hThreadBonus = CreateThread(
		NULL,
		0,
		BonusThread,	//nome da funçao
		(LPVOID)&serverInstance,					//Argumento a ser passado
		CREATE_SUSPENDED,						//flag de criação - SUSPENDIDA
		&serverInstance.serverHandlers.threadHandlers.dwThreadBonus //idThread
	);



	if (serverInstance.serverHandlers.threadHandlers.hThreadConsumer == NULL ||
		serverInstance.serverHandlers.threadHandlers.hThreadBall == NULL ||
		serverInstance.serverHandlers.threadHandlers.hThreadBonus == NULL)
	{
		exit(EXIT_FAILURE);
	}


	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadBonus, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadBall, INFINITE);


	getTopTenRegistry(scoreTopTen);

	//setTopTenRegistry(scoreTopTen);


	freeSyncObject();
	freeMappedMemoryServer(&serverInstance.serverHandlers.sharedMemHandlers);
	freeThreads(serverInstance.serverHandlers.threadHandlers);

	_gettchar();
	return 0;
}