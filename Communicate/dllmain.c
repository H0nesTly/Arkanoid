// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "SharedMemorySetup.h"

//Ponteiro para objetos do jogo
LPVOID lpgSharedMemGame = NULL;
HANDLE hgMapObjGame = NULL;

//Ponteiro para objetos das mensagens
PVOID lpgSharedMemMessage = NULL;
HANDLE hgMapObjMessage = NULL;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
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

