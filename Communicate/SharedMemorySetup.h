#pragma once
#include "stdafx.h"

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientGameMem(LPVOID, HANDLE);

/*Cria handle para ler os dados da memoria mapeada*/
BOOL initClientMessageMemReader(LPVOID, HANDLE);

/*Cria handle para escrever os dados da memoria mapeada*/
BOOL initClientMessageMemWriter(LPVOID, HANDLE);