// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "DllSetup.h"

//Ponteiro para objetos do jogo
LPVOID lpgSharedMemGame = NULL;
HANDLE hgMapObjGame = NULL;

//Ponteiro para objetos das mensagens
LPVOID lpgSharedMemMessage = NULL;
HANDLE hgMapObjMessage = NULL;

//HANDLE PARA sincronizaçao
HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;

//Semaforo para notificar que existe uma nova mensagem para o servidor ler
HANDLE hgSemaphoreWriteToServer = NULL;


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

		if (!initSyncObjects(&hgReadObject, &hgSemaphoreWriteToServer))
			return FALSE;
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		freeMappedMemory(hgMapObjGame,lpgSharedMemGame, hgMapObjMessage, lpgSharedMemMessage);
		freeSyncObjects(hgReadObject, &hgSemaphoreWriteToServer);
        break;
    }
    return TRUE;
}

