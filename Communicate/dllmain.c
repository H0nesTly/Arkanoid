// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "DllSetup.h"

//Ponteiro para objetos do jogo
LPVOID lpgSharedMemGame = NULL;
HANDLE hgMapObjGame = NULL;

//Ponteiro para objetos das mensagens
LPVOID lpgSharedMemMessage = NULL;
HANDLE hgMapObjMessage = NULL;

//HANDLE PARA os eventos
HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//Inicializar memoria partilhada 
		if (!initClientGameMem())
			return FALSE;

		if (!initClientMessageMem())
			return FALSE;
		if (!initSyncObjects())
			return FALSE;


    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		freeMappedMemory();
		freeSyncObjects();
        break;
    }
    return TRUE;
}

