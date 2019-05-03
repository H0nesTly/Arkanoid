#pragma once
#include "stdafx.h"

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMem(HANDLE*, LPVOID*);

/*Cria handle p da memoria mapeada*/
BOOL initClientMessageMem(HANDLE*, LPVOID*);

VOID freeMappedMemory(HANDLE, LPVOID, HANDLE, LPVOID);

BOOL initSyncObjects(HANDLE*, HANDLE*);

VOID freeSyncObjects(HANDLE, HANDLE);