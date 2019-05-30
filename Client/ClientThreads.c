#include "..\Communicate\stdafx.h"
#include "ClientThreads.h"
#include "ClientStructures.h"
#include "Canvas.h"

BOOL bKeepRunning = TRUE;
extern Game* gameObj;
extern HDC memDC;
extern HWND gWnd;

DWORD WINAPI readMessageThread(LPVOID lpArg)
{
	ClientStructure* clientInfo = (ClientStructure*)lpArg;

	//Bug Aqui forçar o felho 
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
	//Sleep(2000);
	//SendMessageDll(&bKeepRunning, TopPlayersMessage);
	return 0;
}

DWORD WINAPI readGameDataThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	HANDLE hEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NAME_EVENT_OBJECT_GAME
	);

	while (bKeepRunning)
	{
		WaitForSingleObject(hEvent, INFINITE);
		ReceiveBroadcast(&bKeepRunning, &gameObj);

		//InvalidateRect(gWnd, NULL, TRUE);

		RedrawWindow(gWnd,0, 0,RDW_INVALIDATE | RDW_UPDATENOW);

		ResetEvent(hEvent);
	}

	return 0;
}

VOID freeThreads(HANDLE hArg[])
{
	CloseHandle(hArg[0]);
	CloseHandle(hArg[1]);
}