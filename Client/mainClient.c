/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A 
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/	
#include <windows.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "..\Communicate\MessageProtocol.h"
#include "ClientStructures.h"

int getLoginMethod()
{
	int Input;

	_tprintf(TEXT("\nInsira Opção de comunicação:"));
	_tprintf(TEXT("\n\t 1 - Memoria Partilhada"));
	_tprintf(TEXT("\n\t 2 - Pipe"));
	_tprintf(TEXT("\n\t 3 - Remoto"));
	_tprintf(TEXT("\n\t 0 - Sair"));

	_tscanf_s(TEXT(" %d"), &Input);

	return Input;
}

int _tmain(int argc, LPTSTR argv[]) 
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
	//LPVOID lpSharedMemGame = NULL;
	//LPVOID	LpSharedMemMessage = NULL;
	//HANDLE hMapObjGame = NULL;
	//HANDLE	hMapObjMessage = NULL;

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	switch (getLoginMethod())
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 0:
	default:
		exit(EXIT_SUCCESS);
	}

	return 0;
}