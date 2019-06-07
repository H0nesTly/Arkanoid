﻿#include "..\Communicate\stdafx.h"
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

	//Bug Aqui for�ar o fecho 
	while (bKeepRunning)
	{
		ReceiveMessage(clientInfo->tcUserName, &bKeepRunning);
	}

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


	while (bKeepRunning)
	{

		ReceiveBroadcast(&bKeepRunning, &gameObj);

		InvalidateRect(gWnd, NULL, FALSE);
		//RedrawWindow(gWnd,0, 0,RDW_INVALIDATE | RDW_UPDATENOW);
	}

	return 0;
}

VOID freeThreads(HANDLE hArg[])
{
	CloseHandle(hArg[0]);
	CloseHandle(hArg[1]);
}