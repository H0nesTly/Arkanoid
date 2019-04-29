// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//Ponteiro para objetos do jogo
static LPVOID lpSharedMemGame = NULL;
static HANDLE hMapObjGame = NULL;

//Ponteiro para objetos das mensagens
static LPVOID lpSharedMemMensage = NULL;
static HANDLE hMapObjMensage = NULL;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	BOOL fInit;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//Inicializar memoria partilhada 
		if (!initClientGameMem(lpSharedMemGame, hMapObjGame))
			return FALSE;

		if (!initClientMessageMem(lpSharedMemMensage, hMapObjMensage))
			return FALSE;



    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

