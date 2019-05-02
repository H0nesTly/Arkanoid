#pragma once
#include "stdafx.h"

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMem();

/*Cria handle p da memoria mapeada*/
BOOL initClientMessageMem();

VOID freeMappedMemory();

BOOL initSyncObjects();

VOID freeSyncObjects();