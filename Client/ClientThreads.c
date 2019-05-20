#include "..\Communicate\stdafx.h"
#include "ClientThreads.h"
#include "ClientStructures.h"


DWORD WINAPI readMessageThread(LPVOID lpArg)
{
	ClientStructure* clientInfo = (ClientStructure*) lpArg;

	while (1)
	{
		ReceiveMessage(clientInfo->tcUserName);
	}

	return 0;
}

DWORD WINAPI readInputThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

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