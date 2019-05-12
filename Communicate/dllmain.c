// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "DllSetup.h"
#include <minwinbase.h>

ClientConnection gClientConnection;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		ZeroMemory(&gClientConnection, sizeof(ClientConnection));
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		freeComponents(&gClientConnection);
        break;
    }
    return TRUE;
}

