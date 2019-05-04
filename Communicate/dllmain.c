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
		if (!initClientGameMem(&hgMapObjGame, &lpgSharedMemGame))
			return FALSE;

		if (!initClientMessageMem(&hgMapObjMessage, &lpgSharedMemMessage))
			return FALSE;

		if (!initSyncObjects(&hgReadObject, &hgWriteObject))
			return FALSE;

		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		freeMappedMemory(hgMapObjGame,lpgSharedMemGame, hgMapObjMessage, lpgSharedMemMessage);
		freeSyncObjects(hgReadObject, hgWriteObject);
        break;
    }
    return TRUE;
}

