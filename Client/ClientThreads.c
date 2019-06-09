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

	do
	{
		ReceiveMessage(clientInfo->tcUserName, &bKeepRunning);
	} while (bKeepRunning);

	return 0;
}

DWORD WINAPI readGameDataThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	do
	{
		ReceiveBroadcast(&bKeepRunning, &gameObj);

		InvalidateRect(gWnd, NULL, FALSE);
		//RedrawWindow(gWnd,0, 0,RDW_INVALIDATE | RDW_UPDATENOW);
	} while (bKeepRunning);

	return 0;
}

VOID freeThreads(HANDLE hArg[])
{
	CloseHandle(hArg[0]);
	CloseHandle(hArg[1]);
}