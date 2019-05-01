/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE EST� ATIVO A
		OP�OA UNICODE LOGO ESTA A CORRER COM UTF-16*/
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

	if(!initSyncObject(handlers.serverSyncObj))
	{
		_tprintf(TEXT("ERRO Criar objetos de sincroniza��o"));
		exit(EXIT_FAILURE);
	}

	WaitForSingleObject(handlers.threadHandlers.hThreadConsumer, INFINITE);
	WaitForSingleObject(handlers.threadHandlers.hThreadProducer, INFINITE);

	freeSyncObject(handlers.serverSyncObj);
	freeMappedMemory(&handlers.sharedMemHandlers);

	return 0;
}