#include "ClientThreads.h"

DWORD WINAPI readMessageThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	while (1)
	{
		ReceiveMessage(TEXT("TEXT"));
	}

	return 0;
}

DWORD WINAPI readInputThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	return 0;
}

VOID freeThreads(HANDLE hArg[])
{
	CloseHandle(hArg[0]);
	CloseHandle(hArg[1]);
}