#pragma once
#include "../Communicate/stdafx.h"
#include  "../Communicate/MessageProtocol.h"

DWORD WINAPI readMessageThread(LPVOID);

DWORD WINAPI readInputThread(LPVOID);

VOID freeThreads(HANDLE []);