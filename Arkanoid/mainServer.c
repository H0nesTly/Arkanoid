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

	ServerHandlers handlers;

	ZeroMemory(&handlers, sizeof(ServerHandlers));

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	handlers.threadHandlers.hThreadConsumer = CreateThread(
		NULL,
		0,
		ConsumerMessageThread,	//nome da funçao
		NULL,					//Argumento a ser passado
		0,						//Flags de criaçao
		&handlers.threadHandlers.dwIdConsumer //idThread
	);

	handlers.threadHandlers.hThreadProducer = CreateThread(
		NULL,
		0,
		ProducerMessageThread,	//nome da funçao
		NULL,					//Argumento a ser passado
		0,						//Flags de criaçao(CREATE_SUSPENDED) -> so começar quando existir algum cliente ligado
		&handlers.threadHandlers.dwIdProducer //idThread
	);

	if (handlers.threadHandlers.hThreadProducer == NULL ||
		handlers.threadHandlers.hThreadConsumer == NULL)
	{
		exit(EXIT_FAILURE);
	}

	if (!intitServerGameMem(handlers.sharedMemHandlers.hMapObjGame,
		handlers.sharedMemHandlers.lpSharedMemGame))
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr!"));
		exit(EXIT_FAILURE);
	}

	if (!intitServerMessageMem(handlers.sharedMemHandlers.hMapObjMessage,
		handlers.sharedMemHandlers.LpSharedMemMessage) )
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr"));
		exit(EXIT_FAILURE);
	}

	if(!initSyncObject())
	{
		_tprintf(TEXT("ERRO Criar objetos de sincronização"));
		exit(EXIT_FAILURE);
	}

	WaitForSingleObject(handlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(handlers.threadHandlers.hThreadProducer, INFINITE);

	freeSyncObject();
	freeMappedMemory(&handlers.sharedMemHandlers);

	return 0;
}