/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Server.h"
#include "ServerStructures.h"

int _tmain(int argc, LPTSTR argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	ServerHandles serverMappedMemory;

	ZeroMemory(&serverMappedMemory, sizeof(ServerHandles));

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif


	if (intitServerGameMem(serverMappedMemory.hMapObjGame,
		serverMappedMemory.lpSharedMemGame) == FALSE)
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr!"));
		exit(EXIT_FAILURE);
	}


	if (intitServerMessageMemReader(serverMappedMemory.hMapObjMessage,
		serverMappedMemory.LpSharedMemMessage) == FALSE)
	{
		_tprintf(TEXT("ERRO Instancia Servidor ja a correr"));
		exit(EXIT_FAILURE);
	}


	return 0;
}