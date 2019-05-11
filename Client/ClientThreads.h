#pragma once
#include "../Communicate/stdafx.h"
#include  "../Communicate/MessageProtocol.h"
#include  "../Communicate/GameStructures.h"

#define NUMBER_OF_CLIENT_THREADS 3

DWORD WINAPI readMessageThread(LPVOID);

DWORD WINAPI readInputThread(LPVOID);

DWORD WINAPI readGameDataThread(LPVOID);

VOID freeThreads(HANDLE []);