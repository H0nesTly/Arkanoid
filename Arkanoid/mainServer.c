/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE EST� ATIVO A
		OP�OA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include "..\Communicate\stdafx.h"

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

	//UNICODE: Por defeito, a consola Windows n�o processa caracteres wide.
	//A maneira mais f�cil para ter esta funcionalidade � chamar _setmode:
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
		_tprintf(TEXT("ERRO Criar objetos de sincroniza��o"));
		exit(EXIT_FAILURE);
	}

	if (!initServerPipeLocal(serverInstance.serverHandlers.namedPipeInstances, MAX_PLAYER_INSTANCES))
	{
		_tprintf(TEXT("ERRO Criar Pipes"));
		exit(EXIT_FAILURE);
	}
	
	loadGameConfiguration(TEXT("config.txt"), &serverConfig);

	serverInstance.serverHandlers.threadHandlers.hThreadConsumer = CreateThread(
		NULL,
		0,
		ConsumerMessageThread,	//nome da fun�ao
		(LPVOID) &serverInstance,					//Argumento a ser passado
		0,						//Flags de cria�ao
		&serverInstance.serverHandlers.threadHandlers.dwIdConsumer //idThread
	);

	serverInstance.serverHandlers.threadHandlers.hThreadBall = CreateThread(
		NULL,
		0,
		BallThread,	//nome da fun�ao
		(LPVOID) serverInstance.serverHandlers.sharedMemHandlers.lpSharedMemGame,					//Argumento a ser passado
		0,						//flag de cira��o
		&serverInstance.serverHandlers.threadHandlers.dwThreadBall //idThread
	);

	if ( serverInstance.serverHandlers.threadHandlers.hThreadConsumer == NULL 
		|| serverInstance.serverHandlers.threadHandlers.hThreadBall == NULL)
	{
		exit(EXIT_FAILURE);
	}


	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(serverInstance.serverHandlers.threadHandlers.hThreadBall, INFINITE);



	getTopTenRegistry(scoreTopTen);
	
	//setTopTenRegistry(scoreTopTen);


	freeSyncObject();
	freeMappedMemoryServer(&serverInstance.serverHandlers.sharedMemHandlers);
	freeThreads(serverInstance.serverHandlers.threadHandlers);



	_gettchar();
	return 0;
}