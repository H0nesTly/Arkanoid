#include "..\Communicate\stdafx.h"
#include "ClientThreads.h"
#include "ClientStructures.h"

BOOL bKeepRunning = TRUE;

DWORD WINAPI readMessageThread(LPVOID lpArg)
{
	ClientStructure* clientInfo = (ClientStructure*) lpArg;

	while (bKeepRunning)
	{
		ReceiveMessage(clientInfo->tcUserName, &bKeepRunning);
	}

	_tprintf(TEXT("\nVou fechar!!"));
	return 0;
}

DWORD WINAPI readInputThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);
	Sleep(2000);
	SendMessageDll(&bKeepRunning, TopPlayersMessage);
	return 0;
}

DWORD WINAPI readGameDataThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	//while (1)
	//{
	//	ReceiveBroadcast();
	//	Sleep(2000);
	//}

	return 0;
}

VOID freeThreads(HANDLE hArg[])
{
	CloseHandle(hArg[0]);
	CloseHandle(hArg[1]);
}