/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A 
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/	
#include <windows.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "ClientStructures.h"
#include "ClientThreads.h"
#include "..\Communicate\MessageProtocol.h"
#include "..\Communicate\DllSetup.h"

LPVOID lpgcSharedMemGame = NULL;
LPVOID lpgcSharedMemMessage = NULL;

int getLoginMethod()
{
	int Input;

	_tprintf(TEXT("\nInsira Opção de comunicação:"));
	_tprintf(TEXT("\n\t 1 - Memoria Partilhada"));
	_tprintf(TEXT("\n\t 2 - Pipe"));
	_tprintf(TEXT("\n\t 3 - Remoto"));
	_tprintf(TEXT("\n\t 0 - Sair"));
	_tprintf(TEXT("\n\t > "));
	_tscanf_s(TEXT(" %d"), &Input);

	return Input;
}

void getUserName(ClientStructure* dest)
{
	_tprintf(TEXT("\n Insira username(ate 20) :"));
	_tscanf_s( TEXT("%19s") , dest->tcUserName , (unsigned)_countof(dest->tcUserName));
}

int _tmain(int argc, LPTSTR argv[]) 
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
	ClientStructure ClientInfo;

	HANDLE hThreads[NUMBER_OF_CLIENT_THREADS];
	DWORD dwThreadsIds[NUMBER_OF_CLIENT_THREADS];

	ZeroMemory(&ClientInfo, sizeof(ClientStructure));

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	getUserName(&ClientInfo);
	switch (getLoginMethod())
	{
	case 1:
		Login(ClientInfo.tcUserName, clientSharedMemoryConnection);
		break;
	case 2:
		Login(ClientInfo.tcUserName, clientNamedPipeLocalConnection);
		break;
	case 3:
		break;
	case 0:
	default:
		exit(EXIT_SUCCESS);
	}

	hThreads[0] = CreateThread(	
		NULL,
		0,
		readInputThread,
		NULL,
		0,
		&dwThreadsIds[0]
	);

	hThreads[1] = CreateThread(
		NULL,
		0,
		readMessageThread,
		NULL,
		0,
		&dwThreadsIds[1]
	);	
	
	hThreads[2] = CreateThread(
		NULL,
		0,
		readGameDataThread,
		NULL,
		0,
		&dwThreadsIds[2]
	);

	WaitForMultipleObjects(NUMBER_OF_CLIENT_THREADS, hThreads, TRUE ,INFINITE);


	freeThreads(hThreads);
	system("PAUSE");
	return 0;
}